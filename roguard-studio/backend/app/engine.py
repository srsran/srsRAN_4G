import asyncio
from collections import deque
import csv
import json
import math
import os
from pathlib import Path
import signal
import sys
import threading
from uuid import uuid4
import psutil
from .storage import now
from .configuration import generate
from .discovery import BINS, REPO, STUDIO, installation, validate

class Engine:
    def __init__(self,store):
        self.store=store; self.proc=None; self.task=None; self.active=None
        self.lock=asyncio.Lock(); self.ingest_lock=threading.RLock(); self.logs=deque(maxlen=1000); self.cursors={}; self.headers={}
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
        return {'execution_adapter':t.get('execution_adapter','auto'),'nodes':sorted([{'id':n['id'],'role':n['kind'],
            'core_id':n.get('core_id'),'radio_environment':n.get('radio_environment'),
            'core_adapter':n.get('core_adapter','srsepc'),'path_attenuation_db':n.get('path_attenuation_db',{}),'config':{
            k:v for k,v in n['config'].items() if n['kind'] != 'enb' or k not in ('cell_id','pci','tac','earfcn')},
            'cells':[{'id':c['id'],'config':c['config'],'rf_attenuation_db':c.get('rf_attenuation_db',0)} for c in n['cells']]}
            for n in t['nodes'] if n['kind'] in kinds],key=lambda n:n['id'])}

    @staticmethod
    def ue_serving_cell(run, entry):
        if run.get('state') not in ('starting','running') or not entry.get('nas_attached') or not entry.get('observed'):
            return None
        stamp=entry.get('last_metric_at')
        if not stamp:return None
        from datetime import datetime, timezone
        if (datetime.now(timezone.utc)-datetime.fromisoformat(stamp.replace('Z','+00:00'))).total_seconds()>8:return None
        observed=entry['observed']['parameters']
        from .models import Topology
        from .network import core_for
        topology=Topology.model_validate(run['topology'])
        matches=[]
        for n in topology.nodes:
            core=core_for(topology,n) if n.kind=='enb' else None
            if entry.get('core_id') and (not core or core.id!=entry['core_id']):continue
            matches.extend((n,c) for c in n.cells if c.config.pci==observed['pci'] and c.config.dl_earfcn==observed['earfcn'])
        if len(matches)!=1:return None
        n,c=matches[0]
        return {'cell_id':c.id,'enb_id':n.id,'name':c.name,**observed,
                'provenance':'fresh srsUE CSV with independent NAS-success evidence matched to unique applied cell; ECI not measured',
                'timestamp':entry['observed']['timestamp']}

    @staticmethod
    def serving_cell(run):
        """Correlate observed PCI/channel with applied cells, never the edited design.

        This is a configuration association, not an independent ECI measurement.
        Ambiguous identity stays unresolved. Stopped runs have historical evidence only.
        """
        if run and run.get('ue_states'):
            return Engine.ue_serving_cell(run,next(iter(run['ue_states'].values())))
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
                            if run.get('rf_plan'):
                                mapping=run['rf_plan'];args=p.cmdline()
                                items=[*mapping['processes'],*mapping.get('ues',[]),*mapping.get('cores',[])]
                                role=next((item['role'] for item in items if str(Path(run['directory'])/item['config']) in args),k)
                                if k=='broker' and mapping.get('adapter')=='matrix-iq-v2':
                                    role=next((c['role'] for c in mapping['channels'] if str(Path(run['directory'])/(c['role']+'.matrix')) in args),k)
                            components.setdefault(role,{}) .update(pid=p.pid,process_state=p.status(),cpu_percent=p.cpu_percent(),memory_bytes=p.memory_info().rss)
            except (psutil.Error,OSError):pass
        node_components={}
        for node in run['topology']['nodes']:
            if node['kind'] not in ('epc','enb','ue'):continue
            role=node['kind']
            if run.get('rf_plan'):
                role=next((p['role'] for p in [*run['rf_plan']['processes'],*run['rf_plan'].get('ues',[]),*run['rf_plan'].get('cores',[])] if p['component_id']==node['id']),role)
            if role in components:node_components[node['id']]=components[role]
        from .models import Topology
        view = {**run, 'topology':Topology.model_validate(run['topology']).model_dump()}
        ue_states={}
        for key,entry in run.get('ue_states',{}).items():
            ue_states[key]={**entry,'state':components.get(entry['role'],{}).get('state','not-started'),
                'nas_attached':entry.get('nas_attached',False) and run['state'] in ('starting','running'),
                'serving_cell':self.ue_serving_cell(run,entry)}
        return {'ues':ue_states,'state':run['state'],'run':view,'components':components,'node_components':node_components,'attached':run.get('attached',False) and run['state'] in ('starting','running'),
                'observed':run.get('observed'),'serving_cell':self.serving_cell(run),'restart_required':bool(current and self.runtime_signature(current)!=self.runtime_signature(run['topology'])),
                'runtime_seconds':max(0,__import__('datetime').datetime.now(__import__('datetime').timezone.utc).timestamp()-run['started_epoch']) if run['state'] in ('starting','running','stopping') else run.get('duration_seconds',0)}
    async def start(self,topology):
        async with self.lock:
            if self.proc and self.proc.returncode is None:raise ValueError('A simulation is already active')
            if self.task and not self.task.done():await asyncio.shield(self.task)
            report=validate(topology)
            if not report['runnable']:raise ValueError(json.dumps(report))
            with self.ingest_lock:
                run_id=str(uuid4()); directory=STUDIO/'var/runs'/run_id
                hashes=generate(REPO,directory,topology)
                self.active={'id':run_id,'topology':topology.model_dump(),'state':'starting','components':{},'attached':False,'observed':None,
                    'rf_plan':report.get('rf_plan'),'started_at':now(),'started_epoch':__import__('time').time(),'software':installation(),'configuration_hashes':hashes,'validation':report,'directory':str(directory)}
                from .network import core_for
                self.active['ue_states']={n.id:{'component_id':n.id,'role':'ue' if i==0 else f'ue-{i+1}',
                    'core_id':core_for(topology,n).id if core_for(topology,n) else None,
                    'nas_attached':False,'rrc_connected':False,'observed':None,'last_metric_at':None,'ip':None}
                    for i,n in enumerate(n for n in topology.nodes if n.kind=='ue')}
                self.active['compatibility']=report.get('compatibility')
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
                    role=event.get('component','ue')
                    entry=next((u for u in self.active.get('ue_states',{}).values() if u['role']==role),None)
                    if entry:
                        entry.update(nas_attached=event['attached'],ip=event.get('ip'))
                        if event.get('attached') and event.get('ip'):
                            entry['last_ip']=event['ip']
                        event['component_id']=entry['component_id']
                    if role=='ue':self.active['attached']=event['attached'];self.active['ue_ip']=event.get('ip')
                    self.persist();self.audit('attachment','Independent native UE NAS evidence changed',**{k:v for k,v in event.items() if k not in ('type','timestamp')})
                elif typ=='broker':
                    role=event.get('component','broker')
                    self.active.setdefault('brokers',{})[role]=event
                    if role=='broker':self.active['broker']=event
                    self.persist();self.publish(event)
                elif typ=='environment':
                    self.active.setdefault('environments',{})[event['core_id']]=event['state']
                    self.persist();self.publish(event)
                elif typ=='sockets':self.active['sockets']=event;self.publish(event)
                elif typ=='phase':
                    self.active['state']=event['state'];self.persist();self.audit('lifecycle',event['message'],run_id=self.active['id'])
                elif typ=='failure':
                    self.active.update(state='failed',error=event['message']);self.persist();self.audit('failure',event['message'],run_id=self.active['id'])
            await proc.wait()
        finally:
            for entry in self.active.get('ue_states',{}).values():entry['nas_attached']=False
            self.active.update(state='failed' if proc.returncode or self.active['state']=='failed' else 'stopped',attached=False,
                finished_at=now(),duration_seconds=__import__('time').time()-self.active['started_epoch'])
            self.active['components']={k:(v if v in ('stopped','failed') else 'failed' if self.active['state']=='failed' else 'stopped') for k,v in self.active['components'].items()}
            self.persist();self.audit('lifecycle',f'Runner exited with code {proc.returncode}',run_id=self.active['id'])
    async def stop(self):
        async with self.lock:
            p=self.proc
            if not p or p.returncode is not None:
                if self.task and not self.task.done():await asyncio.shield(self.task)
                return self.status()
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
    async def stop_core(self, core_id):
        async with self.lock:
            if not self.proc or self.proc.returncode is not None:raise ValueError('No owned experiment is running')
            mapping=self.active.get('rf_plan') or {}
            if mapping.get('adapter')!='matrix-iq-v2' or core_id not in {c['component_id'] for c in mapping['cores']}:
                raise ValueError('Core is not owned by the active matrix experiment')
            self.proc.stdin.write(('stop-core '+core_id+'\n').encode());await self.proc.stdin.drain()
            self.audit('lifecycle','Owned core shutdown requested',component_id=core_id)
            return self.status()

    def ingest(self):
        # The monitor and synchronous capture API may run in different threads.
        # Serialize cursor advancement and prevent run replacement mid-ingestion.
        with self.ingest_lock:self._ingest()

    def _ingest(self):
        if not self.active:return
        directory=Path(self.active['directory'])
        mapping=self.active.get('rf_plan') or {}
        if mapping.get('adapter')=='matrix-iq-v2':
            sources=[(p['kind'],p['metrics'],p['component_id']) for p in [*mapping['ues'],*mapping['processes']]]
        else:
            primary=next((n['id'] for n in self.active['topology']['nodes'] if n['kind']=='ue'),None)
            sources=[('ue','ue_metrics.csv',primary)]
            if mapping:sources += [('enb',p['metrics'],p['component_id']) for p in mapping['processes']]
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
                        entry=self.active.get('ue_states',{}).get(component_id)
                        rrc=row.get('is_attached')==1
                        if entry:
                            entry.update(rrc_connected=rrc,last_metric_at=metric['timestamp'])
                            attached=entry['nas_attached'] and rrc
                        else:
                            # Historical fixture/run compatibility. Newly started
                            # runs always have independent NAS state above.
                            attached=rrc;self.active['attached']=attached
                        if attached and row.get('pci') is not None and row.get('earfcn') is not None:
                            observed={'pci':int(row['pci']),'earfcn':int(row['earfcn'])}
                            if entry:
                                prior=entry.get('observed')
                                entry['observed']={'parameters':observed,'timestamp':metric['timestamp'],'source':'srsUE CSV + independent NAS-success log','run_id':self.active['id']}
                                if entry['role']!='ue' and (not prior or prior['parameters']!=observed):
                                    self.audit('topology-observation','Independent UE serving parameters observed',component_id=component_id,observed=entry['observed'])
                            if not entry or entry['role']=='ue':self.observe(observed,'srsUE metrics CSV + NAS attachment' if entry else 'srsUE metrics CSV (is_attached=1)')
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
