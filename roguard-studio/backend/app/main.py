import asyncio
from contextlib import asynccontextmanager
import csv
import fcntl
import io
import json
import os
from pathlib import Path
import secrets
import zipfile
from uuid import uuid4
from fastapi import FastAPI, HTTPException, Request, Response, WebSocket, WebSocketDisconnect
from fastapi.responses import JSONResponse
from pydantic import ValidationError
from .models import Topology, ExperimentInput, ScenarioInput, Approval, Authorization
from .storage import Store, now
from .discovery import REPO, STUDIO, installation, resources, sockets, validate
from .configuration import generate, sanitized_config
from .engine import Engine
from .integration import router as integration_router
from .credentials import integration_token as configured_integration_token

ORIGINS={'http://127.0.0.1:3000','http://localhost:3000','http://127.0.0.1:8000','http://localhost:8000'}
for port in (os.environ.get('ROGUARD_FRONTEND_PORT'), os.environ.get('ROGUARD_BACKEND_PORT')):
    if port:
        ORIGINS.update({f'http://127.0.0.1:{int(port)}', f'http://localhost:{int(port)}'})
SCENARIOS=[
    ('stable','Stable LTE baseline','Normal','Record the unchanged baseline and actual available measurements.'),
    ('authorized-cell','Authorized temporary cell','Legitimate Change','Represent an approved cell addition. Multiple RF cells are not executed.'),
    ('tac-change','Approved TAC change','Legitimate Change','Record an authorized change intent. Edit matching EPC/eNB TAC and restart to apply.'),
    ('unexpected-plmn','Unexpected PLMN','Suspicious','Synthetic unexpected PLMN observation for downstream analysis.'),
    ('pci-collision','PCI inconsistency','Suspicious','Synthetic collision event; no second RF cell is claimed.'),
    ('unexpected-earfcn','Unexpected EARFCN','Suspicious','Synthetic frequency deviation; actual EARFCN changes require a restart.'),
    ('signal-anomaly','Signal strength anomaly','Suspicious','Synthetic signal event. The eNB AWGN setting is a separate restart-required adapter.'),
    ('broadcast','Inconsistent broadcast information','Suspicious','Synthetic evidence; no forged live SIB logs.'),
    ('neighbor','Neighbor-list inconsistency','Suspicious','Synthetic event; multi-cell neighbor observations are unavailable.'),
    ('downgrade','LTE downgrade event','Suspicious','Synthetic event only. No 2G/3G radio or downgrade execution.'),
    ('authentication','Authentication anomaly','Suspicious','Synthetic event only; subscriber authentication material is never collected.'),
]

def create_app(data_dir=None):
    directory=Path(data_dir or os.environ.get('STUDIO_DATA_DIR') or STUDIO/'var')
    integration_token=configured_integration_token(STUDIO, allow_file=data_dir is None)
    if integration_token and len(integration_token)<32:
        raise ValueError('STUDIO_INTEGRATION_TOKEN must contain at least 32 characters')
    sessions=set()
    @asynccontextmanager
    async def lifespan(app):
        os.umask(0o077)
        directory.mkdir(parents=True,exist_ok=True,mode=0o700)
        lock=(directory/'backend.lock').open('a')
        try:fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
        except BlockingIOError:raise RuntimeError('Another backend already controls this Studio data directory')
        app.state.store=Store(directory);app.state.engine=Engine(app.state.store)
        app.state.engine.monitor_task=asyncio.create_task(app.state.engine.monitor())
        try:yield
        finally:
            await app.state.engine.stop()
            app.state.engine.monitor_task.cancel()
            await asyncio.gather(app.state.engine.monitor_task,return_exceptions=True)
            app.state.store.engine.dispose();lock.close()
    app=FastAPI(title='Roguard LTE Simulation Studio',version='0.1.0',lifespan=lifespan)
    @app.middleware('http')
    async def local_control(request:Request,call_next):
        host=request.headers.get('host','').split(':')[0]
        if host not in ('127.0.0.1','localhost','testserver'):return JSONResponse({'detail':'Localhost service only'},status_code=403)
        origin=request.headers.get('origin')
        if origin and origin not in ORIGINS:return JSONResponse({'detail':'Untrusted origin'},status_code=403)
        if request.url.path.startswith('/integration/'):
            if origin:
                return JSONResponse({'detail':'Machine interface does not accept browser origins'},status_code=403)
            if not integration_token:
                return JSONResponse({'detail':'Integration is disabled'},status_code=503)
            if not secrets.compare_digest(request.headers.get('authorization',''), 'Bearer '+integration_token):
                return JSONResponse({'detail':'Integration bearer token required'},status_code=401,headers={'WWW-Authenticate':'Bearer'})
            if request.method!='GET':
                return JSONResponse({'detail':'Read-only integration'},status_code=405)
            response=await call_next(request)
            response.headers['Cache-Control']='no-store'
            response.headers['X-Content-Type-Options']='nosniff'
            return response
        if request.headers.get('authorization'):
            return JSONResponse({'detail':'Bearer credentials cannot access Studio control routes'},status_code=403)
        if request.method not in ('GET','HEAD','OPTIONS') and origin not in ORIGINS:
            return JSONResponse({'detail':'State changes require an allowed Origin header'},status_code=403)
        if int(request.headers.get('content-length','0'))>2_000_000:return JSONResponse({'detail':'Request too large'},status_code=413)
        if request.url.path.startswith('/api/') and request.url.path!='/api/session' and request.cookies.get('rg_session') not in sessions:
            return JSONResponse({'detail':'Local session required'},status_code=401)
        response=await call_next(request)
        response.headers['Cache-Control']='no-store'
        response.headers['X-Content-Type-Options']='nosniff'
        return response
    @app.post('/api/session')
    def session(response:Response):
        token=secrets.token_urlsafe(32)
        if len(sessions)>1000:sessions.clear()
        sessions.add(token);response.set_cookie('rg_session',token,httponly=True,samesite='strict',path='/api')
        return {'authenticated':True,'scope':'local machine','timestamp':now()}
    def store():return app.state.store
    def engine():return app.state.engine
    def topology(key):
        value=store().get('topology',key)
        if not value:raise HTTPException(404,'Topology not found')
        return Topology.model_validate(value)
    def run(key):
        value=store().get('run',key)
        if not value:raise HTTPException(404,'Run not found')
        return value
    @app.get('/api/system')
    def system():return {**installation(),'resources':resources(),'timestamp':now()}
    @app.get('/api/components')
    def components():return {'runtime':['epc','enb','ue'],'owned_configuration':['cell'],'annotation':['namespace'],'radio':'ZeroMQ only','pause':'UNSUPPORTED','derived_runtime':['broker'],'multi_cell':'NATIVE — two or three total co-channel cells, 50 PRBs, one EPC/UE; handover not enabled'}
    @app.get('/api/capabilities')
    def capability_matrix():
        from .capabilities import capabilities
        return capabilities()
    @app.get('/api/topologies')
    def topologies():return [Topology.model_validate(t).model_dump() for t in store().list('topology')]
    @app.get('/api/topologies/{key}')
    def get_topology(key:str):return topology(key)
    @app.post('/api/configurations/preview')
    def preview(body:Topology):
        import tempfile
        import difflib
        from .configuration import render_rr
        report = validate(body)
        files = {}
        # Full runnable-shape preview uses exactly the same generator as startup.
        if report['valid'] and report['adapter_supported']:
            with tempfile.TemporaryDirectory(prefix='studio-preview-') as tmp:
                directory = Path(tmp)/'run'
                generate(REPO, directory, body)
                files = {p.name:sanitized_config(p) for p in directory.glob('*.conf')}
                # Preview paths are placeholders, not existing applied run resources.
                files = {k:v.replace(str(directory), '<RUN>') for k,v in files.items()}
        else:
            files = {f'{n.id}/rr.conf':render_rr(REPO,n.cells) for n in body.nodes if n.kind == 'enb'}
        active = engine().active
        applied = {}
        if active and active['topology']['id'] == body.id:
            directory = Path(active['directory'])
            applied = {p.name:sanitized_config(p).replace(str(directory), '<RUN>') for p in directory.glob('*.conf')}
        return {'scope':'designed; never applied', 'validation':report, 'rf_plan':report.get('rf_plan'), 'files':files,
                'applied_run_id':active['id'] if applied else None,
                'diffs':{name:''.join(difflib.unified_diff(applied.get(name,'').splitlines(True), text.splitlines(True),
                                                       fromfile=f'applied/{name}', tofile=f'designed/{name}'))
                         for name,text in files.items()},
                'trace':[{'component_id':n.id, 'cell_id':c.id, 'model_path':f'nodes[{ni}].cells[{ci}].config',
                          'native_path':(next((p['role']+'-rr.conf' for p in (report.get('rf_plan') or {}).get('processes',[]) if p['component_id']==n.id),'rr.conf'))+f':cell_list[{ci}]'}
                         for ni,n in enumerate(body.nodes) for ci,c in enumerate(n.cells)],
                'restart_required':True, 'native_execution_verified':False}
    @app.put('/api/topologies/{key}')
    def save_topology(key:str,body:Topology):
        if key!=body.id:raise HTTPException(400,'Path and topology ID differ')
        try:saved=store().save_topology(body.model_dump())
        except ValueError as exc:raise HTTPException(409,str(exc))
        engine().audit('configuration','Designed topology saved',topology_id=key,revision=saved['revision'],capability='RESTART REQUIRED')
        return saved
    @app.post('/api/topologies/{key}/validate')
    def check(key:str):
        report=validate(topology(key));engine().audit('validation','Topology validation completed',topology_id=key,report=report);return report
    @app.get('/api/topologies/{key}/plan')
    def plan(key:str):
        t=topology(key)
        from .radio import is_multi,plan as rf_plan
        return {'rf_plan':rf_plan(t) if is_multi(t) else None,'capability':'RESTART REQUIRED','affected':[{'id':n.id,'name':n.name,'kind':n.kind} for n in t.nodes if n.kind in ('epc','enb','ue','temporary','suspicious')],
            'steps':(['Stop owned UE, then I/Q broker, all eNodeBs, and EPC','Generate per-process native files and explicit RF plan','Prepare owned user/network/mount namespaces','Start EPC and verify MME/SPGW readiness','Start all eNodeBs and the FC32 broker','Start UE; require every eNodeB S1 response and actual NAS attachment'] if is_multi(t) else ['Stop owned UE, eNodeB, then EPC gracefully','Generate fresh private run files','Prepare isolated Linux namespaces','Start EPC and verify MME / SPGW readiness','Start eNodeB and verify ZeroMQ listener','Start UE; verify S1 response and NAS attachment']),
            'note':'The runner reconciles the complete dependency chain. Selected-component restart uses this same full-chain plan. Pause is unsupported.'}
    @app.get('/api/simulations')
    def simulation():return engine().status()
    @app.post('/api/simulations/{key}/start',status_code=202)
    async def start(key:str):
        try:return await engine().start(topology(key))
        except ValueError as exc:
            try:detail=json.loads(str(exc))
            except ValueError:detail=str(exc)
            raise HTTPException(409,detail)
    @app.post('/api/simulations/stop')
    async def stop():return await engine().stop()
    @app.post('/api/simulations/{key}/restart',status_code=202)
    async def restart(key:str):
        t=topology(key);report=validate(t)
        if not report['runnable']:raise HTTPException(409,report)
        await engine().stop()
        try:return await engine().start(t)
        except ValueError as exc:raise HTTPException(409,str(exc))
    @app.get('/api/zeromq')
    def zeromq():
        env=installation()
        return {'enabled':env['zeromq_enabled'],'plugin':env['plugin'],'host':sockets(),'runtime':engine().active.get('sockets') if engine().active else None,
            'mode':'Loopback TCP inside isolated network namespace','rf_plan':engine().active.get('rf_plan') if engine().active else None,
            'directions':(['All cell TX → broker DL sum → UE RX','UE TX → broker UL fan-out → every cell RX'] if engine().active and engine().active.get('rf_plan') else ['eNodeB TX → UE RX','UE TX → eNodeB RX']),
            'options':{'base_srate':[23040000],'fail_on_disconnect':True,'rf_driver':'zmq'},'configuration_changes':'Configuration saved — restart required.'}
    @app.get('/api/logs')
    def logs(component:str|None=None,limit:int=300):
        return [l for l in engine().logs if component is None or l.get('component')==component][-min(max(limit,1),1000):]
    @app.get('/api/events')
    def events():return sorted(store().list('event'),key=lambda x:x['timestamp'])[-500:]
    @app.get('/api/metrics')
    def metrics(run_id:str|None=None,limit:int=1000):
        return [m for m in store().list('metric') if run_id is None or m['run_id']==run_id][-min(max(limit,1),10000):]
    @app.get('/api/scenarios')
    def scenarios():return [{'id':i,'name':n,'category':c,'description':d,'method':'SIMULATED EVENT','prerequisites':'Saved topology and selected target node','expected':'An auditable synthetic event; actual radio metrics remain unchanged'} for i,n,c,d in SCENARIOS]
    @app.post('/api/scenarios')
    def scenario(body:ScenarioInput):
        t=topology(body.topology_id)
        if body.target not in {n.id for n in t.nodes}:raise HTTPException(400,'Scenario target is missing')
        definition=next(s for s in SCENARIOS if s[0]==body.scenario_id)
        item={**body.model_dump(),'id':str(uuid4()),'timestamp':now(),'state':'active','execution_method':'SIMULATED EVENT','ground_truth':definition[2],
            'model_prediction':None,'analyst_decision':None,'source_type':'synthetic','run_id':engine().active['id'] if engine().active else None}
        store().put('scenario',item['id'],item);engine().audit('scenario','Synthetic scenario event recorded',scenario=item)
        return item
    @app.get('/api/scenarios/events')
    def scenario_events():return store().list('scenario')
    @app.post('/api/scenarios/{key}/stop')
    def stop_scenario(key:str):
        s=store().get('scenario',key)
        if not s:raise HTTPException(404,'Scenario not found')
        s.update(state='stopped',stopped_at=now());store().put('scenario',key,s);engine().audit('scenario','Synthetic scenario stopped',scenario_id=key);return s
    @app.get('/api/baselines')
    def baselines():return store().list('baseline')
    @app.get('/api/baselines/authorizations')
    def authorizations():return store().list('authorization')
    @app.post('/api/baselines/{key}/authorize')
    def authorize(key:str,body:Authorization):
        topology(key)
        a={'id':str(uuid4()),'timestamp':now(),'topology_id':key,'parameters':{'pci':body.pci,'earfcn':body.earfcn},'reason':body.reason,'consumed_by':None}
        store().put('authorization',a['id'],a);engine().audit('authorization','Analyst pre-authorized expected radio parameter change',authorization=a);return a
    @app.post('/api/baselines/{key}')
    def baseline(key:str,body:Approval):
        status=engine().status();observed=status.get('observed')
        if not status['attached'] or not observed or status['run']['topology']['id']!=key:raise HTTPException(409,'A current, attached UE observation is required. Designed topology cannot establish an observed baseline.')
        b={**observed,'topology_id':key,'reason':body.reason,'approved_at':now(),'analyst_decision':'Approved baseline'}
        store().put('baseline',key,b);engine().audit('baseline','Analyst approved observed baseline',baseline=b);return b
    @app.get('/api/topology-events')
    def changes():return sorted(store().list('topology-event'),key=lambda x:x['timestamp'],reverse=True)
    @app.post('/api/topology-events/{key}/approve')
    def approve(key:str,body:Approval):
        e=store().get('topology-event',key)
        if not e:raise HTTPException(404,'Event not found')
        if e['source_type']!='real-observation':raise HTTPException(400,'Synthetic events cannot update observed baselines')
        e.update(classification='Legitimate Change',analyst_decision='Approved',reason=body.reason,approved_at=now())
        store().put('topology-event',key,e)
        store().put('baseline',e['topology_id'],{**e['observed'],'topology_id':e['topology_id'],'reason':body.reason,'approved_at':now()})
        engine().audit('baseline','Analyst approved topology change; baseline updated',event=e);return e
    @app.get('/api/experiments')
    def experiments():return [{k:v for k,v in e.items() if k not in ('metrics','logs','configurations','events')} for e in store().list('experiment')]
    @app.post('/api/experiments',status_code=201)
    def capture(body:ExperimentInput):
        t=topology(body.topology_id);engine().ingest();r=engine().active
        if r and r['topology']['id']!=t.id:r=None
        rid=r['id'] if r else None
        logs={};configs={}
        if r:
            for path in Path(r['directory']).glob('*'):
                if path.suffix=='.log':logs[path.name]=path.read_text(errors='replace')
                if path.suffix=='.conf' or path.name=='user_db.csv':configs[path.name]=sanitized_config(path)
        e={'id':str(uuid4()),'timestamp':now(),'name':body.name,'category':body.category,'ground_truth':body.category,'model_prediction':None,'analyst_decision':None,
            'topology':t.model_dump(),'applied_run':r,'software':installation(),'validation':validate(t),'configurations':configs,'logs':logs,
            'metrics':[m for m in store().list('metric') if rid and m['run_id']==rid],
            'events':[e for e in store().list('event') if e.get('topology_id')==t.id or (rid and e.get('run_id')==rid)],
            'scenario_events':[s for s in store().list('scenario') if s['topology_id']==t.id],
            'units':{'timestamps':'UTC ISO 8601','time_ue':'milliseconds since process start','time_enb':'seconds since process start','rsrp':'dBm','dl_snr':'dB','dl_brate':'bit/s','ul_brate':'bit/s','dl_bler':'percent','ul_bler':'percent'},
            'measurement_note':'Timestamp is ingestion time. CSV time preserves engine-relative time. Unavailable values remain null. Snapshot is immutable; captures evidence available at this time.'}
        store().put('experiment',e['id'],e);engine().audit('experiment','Experiment snapshot saved',experiment_id=e['id'],topology_id=t.id);return {'id':e['id'],'name':e['name'],'timestamp':e['timestamp']}
    @app.post('/api/experiments/{key}/restore')
    async def restore(key:str):
        e=store().get('experiment',key)
        if not e:raise HTTPException(404,'Experiment not found')
        await engine().stop()
        t=Topology.model_validate(e['topology']).model_copy(deep=True)
        t.id=str(uuid4());t.revision=0;t.name=(t.name+' · restored')[:100]
        ids={n.id:str(uuid4()) for n in t.nodes}
        for m in t.mobility:m.ue_id=ids[m.ue_id]
        for n in t.nodes:
            old=n.id;n.id=ids[old];n.group=ids.get(n.group) if n.group else None
            for c in n.cells:c.id=str(uuid4())
        for l in t.links:l.id=str(uuid4());l.source=ids[l.source];l.target=ids[l.target]
        saved=store().save_topology(t.model_dump())
        engine().audit('experiment','Experiment design restored into a fresh stopped workspace; source snapshot preserved',experiment_id=key,topology_id=t.id)
        return saved
    def csv_metrics(e):
        buf=io.StringIO();w=csv.writer(buf);w.writerow(['timestamp_utc','run_id','component','source','metric','value'])
        for m in e['metrics']:
            for k,v in m['values'].items():w.writerow([m['timestamp'],m['run_id'],m['component'],m['source'],k,'' if v is None else v])
        return buf.getvalue()
    @app.get('/api/experiments/{key}/export')
    def export_experiment(key:str,format:str='zip'):
        e=store().get('experiment',key)
        if not e:raise HTTPException(404,'Experiment not found')
        if format=='json':data=json.dumps(e,indent=2).encode();mime='application/json'
        elif format=='csv':data=csv_metrics(e).encode();mime='text/csv'
        elif format=='zip':
            buf=io.BytesIO()
            with zipfile.ZipFile(buf,'w',zipfile.ZIP_DEFLATED) as z:
                z.writestr('experiment.json',json.dumps(e,indent=2));z.writestr('metrics.csv',csv_metrics(e))
                for name,txt in e['configurations'].items():z.writestr('configs/'+name,txt)
                for name,txt in e['logs'].items():z.writestr('logs/'+name,txt)
            data=buf.getvalue();mime='application/zip'
        else:raise HTTPException(400,'Use json, csv or zip')
        return Response(data,media_type=mime,headers={'Content-Disposition':f'attachment; filename="roguard-{key}.{format}"'})
    @app.get('/api/configurations/{key}/export')
    def export_topology(key:str):
        t=topology(key)
        return Response(t.model_dump_json(indent=2),media_type='application/json',headers={'Content-Disposition':f'attachment; filename="{t.id}.json"'})
    @app.get('/api/configurations/runs/{key}')
    def configurations(key:str):
        r=run(key);return {p.name:sanitized_config(p) for p in Path(r['directory']).glob('*.conf')}
    @app.websocket('/api/ws')
    async def websocket(ws:WebSocket):
        if ws.headers.get('authorization') or ws.headers.get('origin') not in ORIGINS or ws.cookies.get('rg_session') not in sessions:
            await ws.close(code=1008);return
        await ws.accept();q=asyncio.Queue(maxsize=500);engine().listeners.add(q)
        try:
            while True:
                try:item=await asyncio.wait_for(q.get(),1)
                except asyncio.TimeoutError:item={'type':'status','timestamp':now(),'status':engine().status(),'resources':resources()}
                await ws.send_json(item)
        except (WebSocketDisconnect,RuntimeError):pass
        finally:engine().listeners.discard(q)
    app.include_router(integration_router())
    return app

app=create_app()
