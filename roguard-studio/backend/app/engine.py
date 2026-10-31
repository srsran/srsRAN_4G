import asyncio
from collections import deque
import csv
import json
import math
import os
from pathlib import Path
import signal
import sys
from uuid import uuid4
import psutil
from .storage import now
from .configuration import generate
from .discovery import BINS, REPO, STUDIO, installation, validate

class Engine:
    def __init__(self,store):
        self.store=store; self.proc=None; self.task=None; self.active=None
        self.lock=asyncio.Lock(); self.logs=deque(maxlen=1000); self.cursors={}; self.headers={}
        self.listeners=set(); self.monitor_task=None; self.process_cache={}
        for r in store.list('run'):
            if r['state'] in ('starting','running','stopping'):
                r.update(state='interrupted',finished_at=now(),error='Backend ended before final lifecycle evidence. This record is not treated as running.')
                store.put('run',r['id'],r)
    def audit(self,kind,message,**data):
        if self.active:
            data.setdefault('run_id', self.active['id'])
            data.setdefault('topology_id', self.active['topology']['id'])
        item={'id':str(uuid4()),'timestamp':now(),'type':kind,'message':message,**data}
        self.store.put('event',item['timestamp']+'-'+item['id'],item)
        self.publish(item)
        return item
    def publish(self,item):
        for q in tuple(self.listeners):
            if q.full():
                try:q.get_nowait()
                except asyncio.QueueEmpty:pass
            q.put_nowait(item)
    @staticmethod
    def runtime_signature(t):
        from .models import Topology
        t = Topology.model_validate(t).model_dump()
        kinds={'epc','enb','ue','suspicious','temporary'}
        return {'nodes':sorted([{'id':n['id'],'role':n['kind'],'config':{
            k:v for k,v in n['config'].items() if n['kind'] != 'enb' or k not in ('cell_id','pci','tac','earfcn')},
            'cells':[{'id':c['id'],'config':c['config'],'rf_attenuation_db':c.get('rf_attenuation_db',0)} for c in n['cells']]}
            for n in t['nodes'] if n['kind'] in kinds],key=lambda n:n['id'])}

    @staticmethod
    def serving_cell(run):
        """Correlate observed PCI/channel with applied cells, never the edited design.

        This is a configuration association, not an independent ECI measurement.
        Ambiguous identity stays unresolved. Stopped runs have historical evidence only.
        """
        if not run or run.get('state') not in ('starting','running') or not run.get('attached') or not run.get('observed'):
            return None
        from .models import Topology
        observed = run['observed']['parameters']
        matches = [(n, c) for n in Topology.model_validate(run['topology']).nodes for c in n.cells
                   if c.config.pci == observed['pci'] and c.config.dl_earfcn == observed['earfcn']]
        if len(matches) != 1: return None
        n, c = matches[0]
        return {'cell_id': c.id, 'enb_id': n.id, 'name': c.name,
                'pci': observed['pci'], 'earfcn': observed['earfcn'],
                'provenance': 'unique applied configuration matched to attached srsUE PCI/EARFCN; ECI not measured',
                'timestamp': run['observed']['timestamp']}
    def persist(self):
        if self.active:self.store.put('run',self.active['id'],self.active)
    def status(self):
        run=self.active
        if not run:
            return {'state':'stopped','run':None,'components':{},'attached':False,'observed':None,'serving_cell':None,'restart_required':False}
        current=self.store.get('topology',run['topology']['id'])
        components={k:{'state':v} for k,v in run.get('components',{}).items()}
        # The isolated worker reports namespaced PIDs. Discover only descendants of our owned supervisor.
        if self.proc and self.proc.returncode is None:
            try:
                for discovered in psutil.Process(self.proc.pid).children(recursive=True):
                    p=self.process_cache.setdefault(discovered.pid,discovered)
                    from .radio import BROKER
                    for k,path in {**BINS,'broker':BROKER}.items():
                        if p.exe()==str(path):
                            role=k
                            if k=='enb' and run.get('rf_plan'):
                                args=p.cmdline();role=next((item['role'] for item in run['rf_plan']['processes'] if str(Path(run['directory'])/item['config']) in args),k)
                            components.setdefault(role,{}) .update(pid=p.pid,process_state=p.status(),cpu_percent=p.cpu_percent(),memory_bytes=p.memory_info().rss)
            except (psutil.Error,OSError):pass
        node_components={}
        for node in run['topology']['nodes']:
            if node['kind'] not in ('epc','enb','ue'):continue
            role=node['kind']
            if role=='enb' and run.get('rf_plan'):
                role=next((p['role'] for p in run['rf_plan']['processes'] if p['component_id']==node['id']),role)
            if role in components:node_components[node['id']]=components[role]
        from .models import Topology
        view = {**run, 'topology':Topology.model_validate(run['topology']).model_dump()}
        return {'state':run['state'],'run':view,'components':components,'node_components':node_components,'attached':run.get('attached',False) and run['state'] in ('starting','running'),
                'observed':run.get('observed'),'serving_cell':self.serving_cell(run),'restart_required':bool(current and self.runtime_signature(current)!=self.runtime_signature(run['topology'])),
                'runtime_seconds':max(0,__import__('datetime').datetime.now(__import__('datetime').timezone.utc).timestamp()-run['started_epoch']) if run['state'] in ('starting','running','stopping') else run.get('duration_seconds',0)}
    async def start(self,topology):
        async with self.lock:
            if self.proc and self.proc.returncode is None:raise ValueError('A simulation is already active')
            report=validate(topology)
            if not report['runnable']:raise ValueError(json.dumps(report))
            run_id=str(uuid4()); directory=STUDIO/'var/runs'/run_id
            hashes=generate(REPO,directory,topology)
            self.active={'id':run_id,'topology':topology.model_dump(),'state':'starting','components':{},'attached':False,'observed':None,
                'rf_plan':report.get('rf_plan'),'started_at':now(),'started_epoch':__import__('time').time(),'software':installation(),'configuration_hashes':hashes,'validation':report,'directory':str(directory)}
            self.persist(); self.audit('lifecycle','Starting isolated LTE environment',run_id=run_id)
            env={**os.environ,'ROGUARD_PARENT_NETNS':os.readlink('/proc/self/ns/net')}
            try:
                self.proc=await asyncio.create_subprocess_exec('unshare','--user','--map-root-user','--net','--mount','--fork','--kill-child=SIGTERM',
                    sys.executable,str(STUDIO/'orchestrator/runner.py'),str(directory),stdin=asyncio.subprocess.PIPE,
                    stdout=asyncio.subprocess.PIPE,stderr=asyncio.subprocess.STDOUT,start_new_session=True,env=env,limit=1024*1024)
                self.active['supervisor_pid']=self.proc.pid
                self.active['supervisor_created']=psutil.Process(self.proc.pid).create_time()
                self.persist(); self.task=asyncio.create_task(self.consume(self.proc))
            except Exception as exc:
                self.active.update(state='failed',error=str(exc)); self.persist();raise
            return self.status()
    async def consume(self,proc):
        try:
            async for raw in proc.stdout:
                try:event=json.loads(raw)
                except (ValueError,UnicodeDecodeError):event={'type':'log','component':'runner','message':raw.decode(errors='replace').strip()}
                event.update(timestamp=now(),run_id=self.active['id'])
                typ=event.get('type')
                if typ=='log':self.logs.append(event);self.publish(event)
                elif typ=='component':
                    self.active['components'][event['component']]=event['state'];self.persist();self.publish(event)
                elif typ=='attachment':
                    self.active['attached']=event['attached'];self.active['ue_ip']=event.get('ip');self.persist();self.audit('attachment','UE attachment evidence changed',**{k:v for k,v in event.items() if k not in ('type','timestamp')})
                elif typ=='broker':
                    self.active['broker']=event;self.persist();self.publish(event)
                elif typ=='sockets':self.active['sockets']=event;self.publish(event)
                elif typ=='phase':
                    self.active['state']=event['state'];self.persist();self.audit('lifecycle',event['message'],run_id=self.active['id'])
                elif typ=='failure':
                    self.active.update(state='failed',error=event['message']);self.persist();self.audit('failure',event['message'],run_id=self.active['id'])
            await proc.wait()
        finally:
            self.active.update(state='failed' if proc.returncode or self.active['state']=='failed' else 'stopped',attached=False,
                finished_at=now(),duration_seconds=__import__('time').time()-self.active['started_epoch'])
            self.active['components']={k:(v if v in ('stopped','failed') else 'failed' if self.active['state']=='failed' else 'stopped') for k,v in self.active['components'].items()}
            self.persist();self.audit('lifecycle',f'Runner exited with code {proc.returncode}',run_id=self.active['id'])
    async def stop(self):
        async with self.lock:
            p=self.proc
            if not p or p.returncode is not None:return self.status()
            self.active['state']='stopping';self.persist();self.audit('lifecycle','Graceful shutdown requested',run_id=self.active['id'])
            try:
                p.stdin.write(b'stop\n');await p.stdin.drain()
                await asyncio.wait_for(asyncio.shield(self.task),40)
            except (asyncio.TimeoutError,BrokenPipeError,ConnectionResetError):
                # Never signal a reused PID: require our live child handle and creation time.
                if p.returncode is None:
                    try:
                        if psutil.Process(p.pid).create_time()==self.active['supervisor_created']:
                            os.killpg(p.pid,signal.SIGTERM)
                            try:await asyncio.wait_for(p.wait(),5)
                            except asyncio.TimeoutError:os.killpg(p.pid,signal.SIGKILL);await p.wait()
                    except ProcessLookupError:pass
                if self.task:await self.task
            return self.status()
    def ingest(self):
        if not self.active:return
        directory=Path(self.active['directory'])
        sources=[('ue','ue_metrics.csv',None)]
        if self.active.get('rf_plan'):
            sources += [('enb',p['metrics'],p['component_id']) for p in self.active['rf_plan']['processes']]
        else:sources.append(('enb','enb_metrics.csv',None))
        for component,filename,component_id in sources:
            path=directory/filename
            if not path.exists():continue
            pos=self.cursors.get(str(path),0)
            with path.open() as f:
                if path.stat().st_size<pos:pos=0
                f.seek(pos)
                while True:
                    start=f.tell();line=f.readline()
                    if not line:break
                    if not line.endswith('\n'):f.seek(start);break
                    if line.startswith('#'):continue
                    vals=next(csv.reader([line],delimiter=';'))
                    if not vals:continue
                    if vals[0]=='time':self.headers[str(path)]=vals;continue
                    headers=self.headers.get(str(path))
                    if not headers or len(vals)!=len(headers):continue
                    row={}
                    for k,v in zip(headers,vals):
                        try:n=float(v);row[k]=n if math.isfinite(n) else None
                        except ValueError:row[k]=None
                    metric={'id':str(uuid4()),'timestamp':now(),'run_id':self.active['id'],'component':component,'source':f'srs{component} metrics CSV','values':row}
                    if component_id:metric['component_id']=component_id
                    self.store.put('metric',metric['timestamp']+'-'+metric['id'],metric);self.publish({'type':'metric',**metric})
                    if component=='ue' and self.active['state'] in ('starting','running'):
                        attached=row.get('is_attached')==1
                        self.active['attached']=attached
                        if attached and row.get('pci') is not None and row.get('earfcn') is not None:
                            observed={'pci':int(row['pci']),'earfcn':int(row['earfcn'])}
                            self.observe(observed,'srsUE metrics CSV (is_attached=1)')
                self.cursors[str(path)]=f.tell()
            self.persist()
    def observe(self,observed,source):
        previous=self.active.get('observed')
        if previous and previous.get('parameters')==observed:return
        entry={'parameters':observed,'timestamp':now(),'source':source,'run_id':self.active['id']}
        self.active['observed']=entry
        self.persist()
        baseline=self.store.get('baseline',self.active['topology']['id'])
        unchanged=bool(baseline and baseline['parameters']==observed)
        authorization=next((a for a in self.store.list('authorization') if a['topology_id']==self.active['topology']['id'] and a['parameters']==observed and not a.get('consumed_by')),None)
        classification=None  # Studio records evidence; Roguard owns threat interpretation.
        change={'id':str(uuid4()),'timestamp':now(),'topology_id':self.active['topology']['id'],'run_id':self.active['id'],
            'previous':baseline['parameters'] if baseline else None,'observed':entry,'classification':classification,
            'analyst_decision':None,'model_prediction':None,'ground_truth':None,'source_type':'real-observation',
            'comparison_status':'baseline_missing' if not baseline else 'unchanged' if unchanged else 'previously_authorized' if authorization else 'analysis_required',
            'previously_authorized':bool(authorization),'authorization_id':authorization['id'] if authorization else None}
        if not unchanged:self.store.put('topology-event',change['id'],change)
        if authorization:
            authorization['consumed_by']=change['id'];self.store.put('authorization',authorization['id'],authorization)
        self.audit('topology-observation','Observed radio parameters compared with baseline',classification=classification,observed=entry)
    async def monitor(self):
        while True:
            try:self.ingest()
            except Exception as exc:self.publish({'type':'telemetry-error','message':str(exc),'timestamp':now()})
            await asyncio.sleep(1)
