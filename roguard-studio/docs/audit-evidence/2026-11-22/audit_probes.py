"""Verification-only probes; fixtures below are never native LTE evidence."""
from pathlib import Path
import sys,os,json,io,zipfile,configparser,sqlite3
ROOT=Path('/tmp/studio-audit-20261122'); STUDIO=ROOT/'work/srsRAN_4G/roguard-studio'
sys.path[:0]=[str(STUDIO/'backend'),str(STUDIO/'tests')]
import httpx
from real_smoke import baseline
from app.engine import Engine
from app.storage import Store
from app.main import create_app
from fastapi.testclient import TestClient
result={}
c=httpx.Client(base_url='http://127.0.0.1:18181',trust_env=False,headers={'Origin':'http://127.0.0.1:3000'},timeout=30)
c.post('/api/session').raise_for_status(); s=c.get('/api/simulations').json(); assert s['state']=='stopped'
# Inspect actual three-eNodeB evidence, including an authenticated read-only snapshot.
token=(ROOT/'private-integration-token').read_text(); machine=httpx.Client(base_url='http://127.0.0.1:18181',trust_env=False,headers={'Authorization':'Bearer '+token})
snap=machine.get('/integration/v1/snapshot').json(); all_metrics=c.get('/api/metrics',params={'run_id':s['run']['id'],'limit':10000}).json()
result['actual_multienb_snapshot']={'run_id':s['run']['id'],'native_enb_metrics_components':sorted({m.get('component_id') for m in all_metrics if m['component']=='enb'}),'snapshot_enb_metric_count':len([m for m in snap['metrics'] if m['data']['component']=='enb']),'snapshot_capabilities':snap['capabilities']}
checks={}
for endpoint in ('snapshot','events','stream','schema'):
 with httpx.Client(base_url='http://127.0.0.1:18181',trust_env=False) as bad:
  checks[endpoint]={'missing':bad.get('/integration/v1/'+endpoint).status_code,'wrong':bad.get('/integration/v1/'+endpoint,headers={'Authorization':'Bearer audit-invalid'}).status_code}
  assert set(checks[endpoint].values())=={401}
assert machine.post('/integration/v1/events').status_code==405
assert machine.get('/integration/v1/schema').status_code==200
checks['read_only_post']=405;checks['authorized_schema']=200;result['machine_authentication']=checks
created=c.post('/api/experiments',json={'name':'Audit three-eNodeB export identity check','topology_id':s['run']['topology']['id']});created.raise_for_status();eid=created.json()['id']
exports={f:c.get(f'/api/experiments/{eid}/export',params={'format':f}).content for f in ('json','csv','zip')}
e=json.loads(exports['json']);a=zipfile.ZipFile(io.BytesIO(exports['zip']));assert a.testzip() is None;assert json.loads(a.read('experiment.json'))==e
conf=configparser.ConfigParser();conf.read(Path(s['run']['directory'])/'ue.conf');secrets=[conf['usim']['k'],conf['usim']['opc'],token]
for data in [exports['json'],exports['csv'],*[a.read(n) for n in a.namelist()]]:assert all(secret.lower().encode() not in data.lower() for secret in secrets)
assert c.get(f'/api/experiments/{eid}/export?format=invalid').status_code==400
result['real_exports']={'experiment_id':eid,'zip_crc_ok':True,'json_zip_match':True,'native_metric_count':len(e['metrics']),'private_key_opc_token_absent':True,'csv_header':exports['csv'].decode().splitlines()[0],'csv_loses_enb_component_id':'component_id' not in exports['csv'].decode().splitlines()[0]}
for f,data in exports.items():(ROOT/f'actual-multienb-export.{f}').write_bytes(data)
# RRC-only CSV fixture: no NAS success log or attachment message is delivered.
t=baseline();work=ROOT/'fixture-probes';work.mkdir(exist_ok=True);store=Store(work/'db');engine=Engine(store)
engine.active={'id':'rrc-only-fixture','state':'starting','directory':str(work),'topology':t.model_dump(),'started_epoch':0,'components':{}}
(work/'ue_metrics.csv').write_text('time;earfcn;pci;is_attached\n1000;3350;1;1\n')
engine.ingest();status=engine.status();result['rrc_only_fixture']={'no_nas_log_supplied':True,'status_attached':status['attached'],'serving_cell_resolved':status['serving_cell'] is not None,'warning':'Fixture proves a provenance defect, not native LTE attachment.'}
old='2000-01-01T00:00:00.000Z';engine.active['observed']['timestamp']=old
result['stale_fixture']={'old_observation_still_current_when_no_new_rows':engine.status()['serving_cell'] is not None,'native_experiment':False}
store.engine.dispose()
# Fresh application instance, same isolated database, saves/removes/restores metadata.
dbdir=ROOT/'fixture-service-restart'
with TestClient(create_app(dbdir),headers={'Origin':'http://127.0.0.1:3000'}) as tc:
 tc.post('/api/session');payload=t.model_dump();payload['nodes'][2]['position']={'x':321,'y':456};payload['mobility']=[{'ue_id':'ue-01','start':{'x':0,'y':0},'end':{'x':600,'y':20},'speed':4,'duration':60,'mode':'planned'}]
 # baseline() names the UE ue-01.
 payload['mobility'][0]['ue_id']=payload['nodes'][2]['id']
 saved=tc.put('/api/topologies/default',json=payload);saved.raise_for_status()
 before=tc.get('/api/metrics').json();assert before==[]
with TestClient(create_app(dbdir),headers={'Origin':'http://127.0.0.1:3000'}) as tc:
 tc.post('/api/session');saved=tc.get('/api/topologies/default').json();assert saved['nodes'][2]['position']=={'x':321,'y':456};assert saved['mobility'][0]['speed']==4;assert tc.get('/api/metrics').json()==[]
 saved['mobility']=[];saved['nodes']=saved['nodes'][:2];saved['links']=saved['links'][:1];tc.put('/api/topologies/default',json=saved).raise_for_status();assert len(tc.get('/api/topologies/default').json()['nodes'])==2
with sqlite3.connect(f'file:{dbdir}/studio.sqlite3?mode=ro',uri=True) as db:integrity=db.execute('PRAGMA integrity_check').fetchone()[0]
result['persistence_fixture']={'service_reopen_positions_mobility':True,'removal_persisted':True,'no_fabricated_metrics':True,'sqlite_integrity':integrity}
(ROOT/'audit-probes.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
c.close();machine.close()
