#!/usr/bin/env python3
"""Fixed command runner. Executed ONLY inside new user, network and mount namespaces.
All runtime children share one process group owned by the backend. No shell execution.
"""
import asyncio
import configparser
import json
import os
from pathlib import Path
import re
import signal
import socket
import subprocess
import sys
import time

ROOT=Path(__file__).resolve().parents[2]
BINS={'epc':ROOT/'build/srsepc/src/srsepc','enb':ROOT/'build/srsenb/src/srsenb','ue':ROOT/'build/srsue/src/srsue'}

def emit(kind, **data):
    print(json.dumps({'type':kind,**data}),flush=True)

def redact(line):
    if re.search(r'(?i)(k_asme|k_enb|kasme|kenb|cipher.*key|integrity.*key|authentication vector|\b(?:opc|xres|rand|autn|res|ck|ik|key)\s*[:=])',line):
        return '[authentication material redacted]'
    return re.sub(r'(?i)\b[0-9a-f]{32,}\b','[redacted]',line).rstrip()

class Runner:
    def __init__(self,directory):
        self.dir=directory; self.procs={}; self.tasks=[]; self.evidence={k:'' for k in BINS}
        self.stop=asyncio.Event(); self.failed=False; self.running=False
        self.ue_roles=['ue']; self.intentional_stops=set(); self.readiness={}
        self.rf_plan=json.loads((directory/'rf-plan.json').read_text()) if (directory/'rf-plan.json').exists() else None
        self.enb_roles=[p['role'] for p in self.rf_plan['processes']] if self.rf_plan else ['enb']
        if any(not re.fullmatch(r'enb(?:-[234])?' if self.rf_plan and self.rf_plan.get('adapter')=='matrix-iq-v2' else r'enb(?:-[23])?',k) for k in self.enb_roles): raise ValueError('Invalid native process manifest')
        self.evidence.update({k:'' for k in [*self.enb_roles,'broker']})
    async def command(self,*args):
        p=await asyncio.create_subprocess_exec(*args,stdout=asyncio.subprocess.PIPE,stderr=asyncio.subprocess.STDOUT)
        out,_=await p.communicate()
        if p.returncode: raise RuntimeError(f'{args[0]} failed: {out.decode(errors="replace").strip()}')
    async def read(self,k,p):
        log=self.dir/f'{k}.log'
        with log.open('a') as f:
            async for raw in p.stdout:
                line=redact(raw.decode(errors='replace'))
                f.write(line+'\n'); f.flush()
                self.evidence[k]=(self.evidence[k]+line+'\n')[-100000:]
                ready=self.readiness.setdefault(k,set())
                for token in ('MME Initialized','SP-GW Initialized','S1Setup procedure completed successfully'):
                    if token in line:ready.add(token)
                if '"event":"ready"' in line:ready.add('broker-ready')
                if re.search(r'(?i)(S1SetupResponse|S1 Setup Response|S1 setup completed|S1Setup.*success)',line):ready.add('s1-setup')
                emit('log',component=k,message=line)
                if k=='broker' or k.startswith('broker-'):
                    try: payload=json.loads(line)
                    except ValueError: payload=None
                    if payload: emit('broker',component=k,provenance='native FC32 I/Q broker sample counters',data=payload)
                if k in self.ue_roles:
                    match=re.search(r'Network attach successful\. IP: ([\d.]+)',line)
                    if match:
                        ready.add('NAS-attached')
                        emit('attachment',component=k,attached=True,ip=match[1],source='srsUE NAS attach-success log')
                    if re.search(r'(?i)(Network detach|EMM.*DEREGISTERED|Network attach failed|Attach failed)',line):
                        ready.discard('NAS-attached')
                        emit('attachment',component=k,attached=False,ip=None,source='srsUE NAS log')
        await p.wait()
        if not self.stop.is_set() and k not in self.intentional_stops:
            self.failed=True
            emit('component',component=k,state='failed')
            emit('failure',component=k,message=f'{k} exited unexpectedly with code {p.returncode}')
            self.stop.set()
    async def spawn(self,k):
        emit('component',component=k,state='starting')
        if k=='broker':
            args=[str(ROOT/'roguard-studio/var/tools/rf_broker'),str(self.rf_plan['ue_tx']),str(self.rf_plan['broker_dl'])]
            for path in self.rf_plan['paths']: args.extend(map(str,[path['native_tx'],path['broker_ul'],path['attenuation_db']]))
        else: args=[str(BINS['enb' if k in self.enb_roles else k]),str(self.dir/f'{k}.conf')]
        p=await asyncio.create_subprocess_exec('stdbuf','-oL','-eL',*args,cwd=self.dir,
            stdin=asyncio.subprocess.PIPE,stdout=asyncio.subprocess.PIPE,stderr=asyncio.subprocess.STDOUT,limit=1024*1024)
        self.procs[k]=p; self.tasks.append(asyncio.create_task(self.read(k,p)))
    async def until(self,predicate,label,timeout=45):
        deadline=time.monotonic()+timeout
        while not predicate():
            if self.stop.is_set(): raise RuntimeError(f'Stopped while waiting for {label}')
            if time.monotonic()>deadline: raise RuntimeError(f'Readiness timeout: {label}. Inspect component logs.')
            await asyncio.sleep(.1)
    def tx_ready(self,port):
        # TCP connect would consume a ZeroMQ peer; inspect listen table without touching RF sockets.
        try:
            lines=Path('/proc/net/tcp').read_text().splitlines()[1:]
            return any(int(l.split()[1].split(':')[1],16)==port and l.split()[3]=='0A' for l in lines)
        except OSError: return False
    async def shutdown(self):
        self.stop.set()
        for k in (['ue','broker',*reversed(self.enb_roles),'epc'] if self.rf_plan else ['ue','enb','epc']):
            p=self.procs.get(k)
            if p and p.returncode is None:
                emit('component',component=k,state='stopping')
                p.send_signal(signal.SIGINT)
                try: await asyncio.wait_for(p.wait(),8)
                except asyncio.TimeoutError:
                    p.terminate()
                    try: await asyncio.wait_for(p.wait(),3)
                    except asyncio.TimeoutError: p.kill(); await p.wait()
                emit('component',component=k,state='stopped')
        for task in self.tasks:
            if not task.done(): task.cancel()
        await asyncio.gather(*self.tasks,return_exceptions=True)
    async def control(self):
        reader=asyncio.StreamReader()
        transport,_=await asyncio.get_running_loop().connect_read_pipe(lambda:asyncio.StreamReaderProtocol(reader),sys.stdin)
        try:
            await reader.readline()
            self.stop.set()
        finally:transport.close()
    async def heartbeat(self):
        while not self.stop.is_set():
            try:
                p=await asyncio.create_subprocess_exec('ss','-tnp',stdout=asyncio.subprocess.PIPE,stderr=asyncio.subprocess.DEVNULL)
                out,_=await p.communicate()
                emit('sockets',source='ss inside isolated LTE network namespace',text=out.decode(errors='replace')[:10000])
            except OSError: pass
            await asyncio.sleep(2)
    async def run(self):
        loop=asyncio.get_running_loop()
        for sig in (signal.SIGINT,signal.SIGTERM): loop.add_signal_handler(sig,self.stop.set)
        self.tasks.append(asyncio.create_task(self.control()))
        try:
            # These mutations are confined to our private mount/network namespaces.
            await self.command('mount','--make-rprivate','/')
            await self.command('mount','-t','tmpfs','-o','mode=755','tmpfs','/run')
            await self.command('ip','link','set','lo','up')
            await self.command('ip','netns','add','rgstudio-ue')
            await self.command('ip','netns','exec','rgstudio-ue','ip','link','set','lo','up')
            emit('phase',state='starting',message='Private loopback and UE namespace prepared')
            await self.spawn('epc')
            await self.until(lambda:all(s in self.readiness.get('epc',set()) for s in ('MME Initialized','SP-GW Initialized')),'EPC MME + SPGW readiness')
            emit('component',component='epc',state='ready')
            if self.rf_plan:
                for role in self.enb_roles: await self.spawn(role)
                await self.spawn('broker')
                await self.until(lambda:'broker-ready' in self.readiness.get('broker',set()),'I/Q broker listeners')
                emit('component',component='broker',state='ready')
                await self.spawn('ue')
                await self.until(lambda:'NAS-attached' in self.readiness.get('ue',set()),'UE NAS attachment through I/Q broker',90)
                for role in self.enb_roles:
                    await self.until(lambda role=role:'s1-setup' in self.readiness.get(role,set()),f'{role} S1 setup')
                    emit('component',component=role,state='ready')
                emit('component',component='ue',state='ready')
            else:
                await self.spawn('enb')
                c=configparser.ConfigParser(); c.read(self.dir/'enb.conf')
                port=int(re.search(r'tx_port=tcp://127\.0\.0\.1:(\d+)',c['rf']['device_args'])[1])
                await self.until(lambda:self.tx_ready(port),'eNodeB ZeroMQ TX listen socket')
                emit('component',component='enb',state='radio-listening')
                # ZMQ startup can wait for its peer: launch UE after the RF listener, then verify S1 and attachment.
                await self.spawn('ue')
                await self.until(lambda:'NAS-attached' in self.readiness.get('ue',set()),'UE NAS attachment',90)
                await self.until(lambda:'s1-setup' in self.readiness.get('enb',set()),'eNodeB S1 setup response')
                for k in ('enb','ue'): emit('component',component=k,state='ready')
            self.running=True; emit('phase',state='running',message='EPC ready; eNodeB S1 established; UE NAS attach verified')
            self.tasks.append(asyncio.create_task(self.heartbeat()))
            await self.stop.wait()
        except Exception as exc:
            self.failed=True; emit('failure',message=str(exc))
        finally:
            await self.shutdown()
            emit('phase',state='failed' if self.failed else 'stopped',message='Owned LTE processes stopped; isolated namespaces released on exit')
        return 1 if self.failed else 0

class MatrixRunner(Runner):
    def __init__(self,directory):
        super().__init__(directory)
        self.manifest={p['role']:p for p in [*self.rf_plan['cores'],*self.rf_plan['processes'],*self.rf_plan['ues']]}
        self.ue_roles=[p['role'] for p in self.rf_plan['ues']]
        self.core_roles=[p['role'] for p in self.rf_plan['cores']]
        self.broker_roles=[p['role'] for p in self.rf_plan['channels']]
        for channel in self.rf_plan['channels']:self.manifest[channel['role']]={**channel,'kind':'broker'}
        for role in self.manifest:
            if not re.fullmatch(r'(?:(?:epc|enb|broker)(?:-[2-4])?|ue(?:-(?:[2-9]|[1-9][0-9]+))?)',role):raise ValueError('Invalid matrix process role')
        self.evidence={role:'' for role in self.manifest}

    async def spawn(self,k):
        p=self.manifest[k]
        if p['kind']=='broker':args=[str(ROOT/'roguard-studio/var/tools/rf_broker'),'--matrix',str(self.dir/(k+'.matrix'))]
        else:args=[str(BINS[p['kind']]),str(self.dir/p['config'])]
        if p['network_namespace']:args=['ip','netns','exec',p['network_namespace'],*args]
        emit('component',component=k,state='starting',component_id=p.get('component_id'))
        child=await asyncio.create_subprocess_exec('stdbuf','-oL','-eL',*args,cwd=self.dir,stdin=asyncio.subprocess.PIPE,stdout=asyncio.subprocess.PIPE,stderr=asyncio.subprocess.STDOUT,limit=1024*1024)
        self.procs[k]=child;self.tasks.append(asyncio.create_task(self.read(k,child)))

    async def stop_roles(self,roles):
        self.intentional_stops.update(roles)
        for k in roles:
            child=self.procs.get(k)
            if child and child.returncode is None:
                emit('component',component=k,state='stopping')
                child.send_signal(signal.SIGINT)
                try:await asyncio.wait_for(child.wait(),8)
                except asyncio.TimeoutError:
                    child.terminate()
                    try:await asyncio.wait_for(child.wait(),3)
                    except asyncio.TimeoutError:child.kill();await child.wait()
                emit('component',component=k,state='stopped')
            if k in self.ue_roles:emit('attachment',component=k,attached=False,ip=None,source='owned UE process stopped')

    async def shutdown(self):
        self.stop.set()
        await self.stop_roles([*reversed(self.ue_roles),*reversed(self.broker_roles),*reversed(self.enb_roles),*reversed(self.core_roles)])
        for task in self.tasks:
            if not task.done():task.cancel()
        await asyncio.gather(*self.tasks,return_exceptions=True)

    async def control(self):
        reader=asyncio.StreamReader()
        transport,_=await asyncio.get_running_loop().connect_read_pipe(lambda:asyncio.StreamReaderProtocol(reader),sys.stdin)
        try:
            while True:
                raw=await reader.readline()
                if not raw or raw.strip()==b'stop':self.stop.set();return
                line=raw.decode().strip()
                if line.startswith('stop-core '):
                    core=line.removeprefix('stop-core ')
                    if core not in {p['component_id'] for p in self.rf_plan['cores']}:raise ValueError('Unknown owned core')
                    roles=[k for k,p in self.manifest.items() if p.get('core_id')==core or p.get('component_id')==core and p['kind']=='epc']
                    order=[k for k in [*reversed(self.ue_roles),*reversed(self.broker_roles),*reversed(self.enb_roles),*reversed(self.core_roles)] if k in roles]
                    await self.stop_roles(order)
                    emit('environment',core_id=core,state='stopped')
        finally:transport.close()

    async def run(self):
        loop=asyncio.get_running_loop()
        for sig in (signal.SIGINT,signal.SIGTERM):loop.add_signal_handler(sig,self.stop.set)
        self.tasks.append(asyncio.create_task(self.control()))
        try:
            await self.command('mount','--make-rprivate','/')
            await self.command('mount','-t','tmpfs','-o','mode=755','tmpfs','/run')
            await self.command('ip','link','set','lo','up')
            namespaces=[p['network_namespace'] for p in self.rf_plan['cores'] if p['network_namespace']]
            namespaces += [p['tun_namespace'] for p in self.rf_plan['ues']]
            for namespace in namespaces:
                await self.command('ip','netns','add',namespace)
                await self.command('ip','netns','exec',namespace,'ip','link','set','lo','up')
            emit('phase',state='starting',message='Owned core and independent UE TUN namespaces prepared')
            for role in self.core_roles:
                await self.spawn(role)
                await self.until(lambda role=role:all(s in self.readiness.get(role,set()) for s in ('MME Initialized','SP-GW Initialized')),role+' MME/SPGW')
                emit('component',component=role,state='ready')
            for role in self.enb_roles:await self.spawn(role)
            for role in self.broker_roles:
                await self.spawn(role)
                await self.until(lambda role=role:'broker-ready' in self.readiness.get(role,set()),role+' matrix listeners')
                emit('component',component=role,state='ready')
            for role in self.ue_roles:await self.spawn(role)
            for role in self.ue_roles:
                await self.until(lambda role=role:'NAS-attached' in self.readiness.get(role,set()),role+' independent NAS attachment',120)
                emit('component',component=role,state='ready')
            for role in self.enb_roles:
                await self.until(lambda role=role:'S1Setup procedure completed successfully' in self.readiness.get(role,set()),role+' S1 setup')
                emit('component',component=role,state='ready')
            self.running=True;emit('phase',state='running',message='All owned cores, eNodeB S1 connections and independent UE NAS attachments verified')
            self.tasks.append(asyncio.create_task(self.heartbeat()));await self.stop.wait()
        except Exception as exc:
            self.failed=True;emit('failure',message=str(exc))
        finally:
            await self.shutdown();emit('phase',state='failed' if self.failed else 'stopped',message='All owned native processes stopped; isolated namespaces released')
        return 1 if self.failed else 0


if __name__=='__main__':
    # Refuse execution in the backend's namespaces, including accidental direct invocation.
    guard=os.environ.get('ROGUARD_PARENT_NETNS')
    if not guard or os.readlink('/proc/self/ns/net')==guard or os.geteuid()!=0:
        sys.exit('Runner requires a newly isolated user/network/mount namespace')
    run_dir=Path(sys.argv[1]).resolve()
    allowed=ROOT/'roguard-studio/var/runs'
    if not run_dir.is_relative_to(allowed.resolve()) or run_dir.parent!=allowed.resolve():
        sys.exit('Run directory outside fixed Studio run root')
    manifest=run_dir/'rf-plan.json'
    matrix=manifest.exists() and json.loads(manifest.read_text()).get('adapter')=='matrix-iq-v2'
    sys.exit(asyncio.run((MatrixRunner if matrix else Runner)(run_dir).run()))
