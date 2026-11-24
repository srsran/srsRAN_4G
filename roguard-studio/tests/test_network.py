"""Contract/configuration tests. Native acceptance is tests/real_network.py."""
import configparser
import json
from pathlib import Path
import pytest
from app.models import Topology,Component,Config,Cell,CellConfig
from app.compatibility import assess,fingerprint,PRB_RATES,admit
from app.network import plan,generate,implementation_hash
from app.templates import build_template,catalog,instantiate
from app.discovery import REPO
from app.configuration import sanitized_config


def test_matrix_subscribers_are_distinct_and_secrets_stay_private(tmp_path):
    t=build_template('two-ues');directory=tmp_path/'run';hashes=generate(REPO,directory,t)
    manifest=json.loads((directory/'rf-plan.json').read_text())
    assert len(manifest['ues'])==2 and len({p['tun_namespace'] for p in manifest['ues']})==2
    assert len({p['ue_tx'] for p in manifest['ues']})==2
    records=(directory/'epc-user_db.csv').read_text().splitlines()
    assert len(records)==2 and (directory/'epc-user_db.csv').stat().st_mode&0o777==0o600
    private=[]
    for process,record in zip(manifest['ues'],records):
        c=configparser.ConfigParser();c.read(directory/process['config']);fields=record.split(',')
        assert c['usim']['imsi']==fields[2] and c['usim']['k']==fields[3] and c['usim']['opc']==fields[5]
        private.extend([fields[3],fields[5]])
        assert fields[3] not in sanitized_config(directory/process['config'])
    assert len(set(private))==4
    assert all(secret not in json.dumps(t.model_dump()) for secret in private)
    assert 'omitted' in sanitized_config(directory/'epc-user_db.csv')
    assert 'rf-plan.json' in hashes and 'broker.matrix' in hashes


def test_duplicate_imsi_and_unassigned_core_are_invalid():
    t=build_template('two-ues');ues=[n for n in t.nodes if n.kind=='ue'];ues[1].config.imsi=ues[0].config.imsi
    assert any('unique synthetic IMSI' in e for e in assess(t)['errors'])
    t=build_template('two-cores');t.nodes[-1].core_id=None
    assert any('explicit EPC' in e for e in assess(t)['errors'])


def test_frequency_groups_are_disjoint_and_bandwidth_rates_are_real():
    t=build_template('two-frequencies');p=plan(t)
    assert len(p['channels'])==2
    assert {c['earfcn'] for c in p['channels']}=={3350,3400}
    assert all(len(c['ues'])==1 and len(c['paths'])==1 for c in p['channels'])
    assert {u['component_id'] for c in p['channels'] for u in c['ues']}=={n.id for n in t.nodes if n.kind=='ue'}
    t=build_template('bandwidth-25');p=plan(t)
    assert p['sample_rate']==5760000 and p['channels'][0]['block_samples']==5760
    assert PRB_RATES[50]==11520000 and PRB_RATES[100]==23040000
    t.nodes[1].config.n_prb=75
    assert not assess(t)['adapter_supported']


def test_four_cells_need_distinct_supported_process_ownership():
    t=build_template('four-cells');report=assess(t,check_environment=False)
    assert report['valid'] and report['adapter_supported']
    assert len(plan(t)['paths'])==4 and len(plan(t)['processes'])==4
    t.nodes[1].cells.extend([Cell(id='extra-a',config=CellConfig(cell_id=2,rf_port=1,pci=21)),Cell(id='extra-b',config=CellConfig(cell_id=3,rf_port=2,pci=22)),Cell(id='extra-c',config=CellConfig(cell_id=4,rf_port=3,pci=23))])
    assert not assess(t,check_environment=False)['adapter_supported']


def test_compatibility_is_not_inferred_from_model_or_compile_time_limit(monkeypatch,tmp_path):
    import app.compatibility as registry
    t=build_template('two-ues');empty=tmp_path/'profiles.json';monkeypatch.setattr(registry,'EVIDENCE',empty)
    report=assess(t,check_environment=False)
    assert report['compatibility']['status']=='Supported by code but not yet native-verified'
    assert not report['compatibility']['native_verified']
    version=report['compatibility']['software_version']
    profile={'fingerprint':fingerprint(t),'native_version':version,'native_binary_sha256':__import__('app.discovery',fromlist=['installation']).installation()['native_binary_sha256'],'implementation_sha256':implementation_hash(),'result':'passed','broker_binary_sha256':__import__('app.compatibility',fromlist=['broker_tool']).broker_tool()['sha256'],'test_id':'PROTOCOL-FIXTURE-NOT-NATIVE'}
    empty.write_text(json.dumps({'profiles':[profile]}))
    assert assess(t,check_environment=False)['compatibility']['native_verified']
    profile['native_version']='other-version';empty.write_text(json.dumps({'profiles':[profile]}))
    assert not assess(t,check_environment=False)['compatibility']['native_verified']
    profile['native_version']=version;profile['implementation_sha256']='old-code';empty.write_text(json.dumps({'profiles':[profile]}))
    assert not assess(t,check_environment=False)['compatibility']['native_verified']


def test_missing_prerequisite_is_separate_from_supported_code(monkeypatch):
    from app.discovery import installation
    env={**installation(),'tun_available':False}
    monkeypatch.setattr('app.discovery.installation',lambda:env)
    report=assess(build_template('two-ues'))
    assert report['valid'] and report['adapter_supported'] and not report['environment_ready']
    assert report['compatibility']['status']=='Blocked by missing prerequisites'


def test_templates_are_additive_versioned_and_have_consistent_admission(client):
    rows=client.get('/api/network-templates').json()
    assert len(rows)>=5 and all(r['version']==1 and r['required_adapter'] for r in rows)
    before=client.get('/api/topologies').json()
    for row in rows:
        old=json.dumps(row['topology'],sort_keys=True)
        response=client.post(f'/api/network-templates/{row["id"]}/projects')
        assert response.status_code==201
        created=Topology.model_validate(response.json());original=Topology.model_validate(row['topology'])
        assert created.id!=original.id
        assert not ({n.id for n in created.nodes}&{n.id for n in original.nodes})
        assert fingerprint(created)==fingerprint(original)==row['compatibility']['profile_fingerprint']
        assert json.dumps(row['topology'],sort_keys=True)==old
        report=client.post('/api/compatibility',json=created.model_dump()).json()
        assert report['valid'] and report['adapter_supported']
    assert len(client.get('/api/topologies').json())==len(before)+len(rows)
    assert client.get('/api/simulations').json()['state']=='stopped'


def test_external_core_cannot_silently_run_bundled_epc(tmp_path):
    from app.configuration import generate as public_generate
    t=build_template('single-cell');t.execution_adapter='auto';t.nodes[0].core_adapter='open5gs'
    report=assess(t,check_environment=False)
    assert not report['adapter_supported']
    with pytest.raises(ValueError,match='Open5GS'):public_generate(REPO,tmp_path/'external',t)
    assert not (tmp_path/'external').exists()


def test_core_isolation_and_owned_stop_admission(client):
    t=build_template('two-cores');mapping=plan(t)
    assert mapping['cores'][0]['network_namespace'] is None
    assert mapping['cores'][1]['network_namespace']=='rgstudio-core-2'
    assert len({u['tun_namespace'] for u in mapping['ues']})==2
    assert all(len({p['core_id'] for p in c['paths']})==1 for c in mapping['channels'])
    assert client.post('/api/simulations/cores/not-owned/stop').status_code==409


def test_nas_evidence_is_independent_per_ue_and_required_for_serving(tmp_path):
    from app.storage import Store,now
    from app.engine import Engine
    t=build_template('two-ues');store=Store(tmp_path/'db');engine=Engine(store)
    engine.active={'id':'fixture','state':'starting','started_epoch':0,'components':{},'topology':t.model_dump(),'directory':str(tmp_path),'ue_states':{}}
    for i,u in enumerate(n for n in t.nodes if n.kind=='ue'):
        entry={'component_id':u.id,'role':'ue' if i==0 else 'ue-2','core_id':u.core_id,'nas_attached':i==0,'last_metric_at':now(),
               'observed':{'parameters':{'pci':1,'earfcn':3350},'timestamp':now()}}
        engine.active['ue_states'][u.id]=entry
    status=engine.status();values=list(status['ues'].values())
    assert values[0]['serving_cell'] and values[1]['serving_cell'] is None
    values=list(engine.active['ue_states'].values());values[0]['last_metric_at']='2000-01-01T00:00:00Z'
    assert engine.serving_cell(engine.active) is None
    store.engine.dispose()

@pytest.mark.asyncio
async def test_readiness_markers_survive_bounded_log_rotation(tmp_path):
    import importlib.util,asyncio,sys
    source=Path(__file__).resolve().parents[1]/'orchestrator/runner.py'
    spec=importlib.util.spec_from_file_location('readiness_runner',source);module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
    runner=module.Runner(tmp_path);runner.stop.set()
    child=await asyncio.create_subprocess_exec(sys.executable,'-c',"print('S1Setup procedure completed successfully');print(('noise'*100+'\\n')*400)",stdout=asyncio.subprocess.PIPE)
    await runner.read('enb',child)
    assert 'S1Setup procedure completed successfully' not in runner.evidence['enb']
    assert 'S1Setup procedure completed successfully' in runner.readiness['enb']


def test_band_edge_is_not_accepted_only_because_earfcn_is_in_range():
    t=build_template('single-cell');t.nodes[1].cells[0].config.dl_earfcn=2750;t.nodes[-1].config.earfcn=2750
    report=assess(t,check_environment=False)
    assert not report['valid'] and any('Band 7 boundaries' in message for message in report['errors'])


def test_explicit_legacy_never_ignores_new_core_or_path_settings():
    t=build_template('single-cell');t.nodes[0].core_adapter='open5gs'
    assert not assess(t,check_environment=False)['adapter_supported']
    t.nodes[0].core_adapter='srsepc';t.nodes[-1].path_attenuation_db={t.nodes[1].cells[0].id:20}
    assert not assess(t,check_environment=False)['adapter_supported']
    t.execution_adapter='auto';assert assess(t,check_environment=False)['compatibility']['adapter']=='matrix-iq-v2'


def test_exact_profile_fingerprint_survives_http_numeric_roundtrip():
    t=build_template('two-ues-two-enbs')
    normalized=Topology.model_validate_json(t.model_dump_json())
    assert fingerprint(t)==fingerprint(normalized)


def test_stale_broker_is_a_prerequisite_blocker_not_native_verification(monkeypatch):
    import app.compatibility as registry
    monkeypatch.setattr(registry,'broker_tool',lambda:{'ready':False,'sha256':None})
    report=assess(build_template('two-ues'))
    assert report['valid'] and report['adapter_supported'] and not report['runnable']
    assert report['compatibility']['status']=='Blocked by missing prerequisites'
    assert any('stale' in e for e in report['runtime_blockers'])


def test_insufficient_memory_blocks_runtime_but_not_configuration_preview(monkeypatch):
    from types import SimpleNamespace
    monkeypatch.setattr('psutil.virtual_memory',lambda:SimpleNamespace(available=0))
    report=assess(build_template('two-ues'))
    assert report['valid'] and report['adapter_supported'] and not report['runnable']
    assert report['compatibility']['status']=='Blocked by missing prerequisites'
    assert any('available memory' in e for e in report['runtime_blockers'])
    assert assess(build_template('two-ues'),check_environment=False)['runnable']


def test_concurrent_capture_and_monitor_ingest_each_native_row_once(tmp_path,baseline,monkeypatch):
    from concurrent.futures import ThreadPoolExecutor
    import threading,time
    from app.storage import Store
    from app.engine import Engine
    store=Store(tmp_path/'db');engine=Engine(store)
    engine.active={'id':'concurrent-fixture','state':'running','started_epoch':0,'topology':baseline.model_dump(),'directory':str(tmp_path),'components':{}}
    (tmp_path/'ue_metrics.csv').write_text('time;earfcn;pci;is_attached\n1000;3350;1;1\n')
    original=store.put
    def put(kind,key,value):
        if kind=='metric':time.sleep(.03)
        return original(kind,key,value)
    monkeypatch.setattr(store,'put',put)
    barrier=threading.Barrier(2)
    def ingest():barrier.wait();engine.ingest()
    with ThreadPoolExecutor(max_workers=2) as pool:
        futures=[pool.submit(ingest) for _ in range(2)]
        for future in futures:future.result(timeout=5)
    assert len(store.list('metric'))==1
    store.engine.dispose()


@pytest.mark.parametrize('template_id',['two-ues-two-enbs','two-frequencies','two-cores'])
def test_advanced_restore_remaps_core_paths_and_radio_environments(client,template_id):
    from app.network import core_for,environment_id
    t=build_template(template_id)
    for u in (n for n in t.nodes if n.kind=='ue'):
        core=core_for(t,u);enb=next(n for n in t.nodes if n.kind=='enb' and n.core_id==core.id and any(c.config.dl_earfcn==u.config.earfcn for c in n.cells))
        u.radio_environment=environment_id(core.id,u.config.earfcn,enb.config.n_prb)
    before=fingerprint(t)
    assert client.put(f'/api/topologies/{t.id}',json=t.model_dump()).status_code==200
    capture=client.post('/api/experiments',json={'name':'Advanced restore regression','topology_id':t.id})
    assert capture.status_code==201;eid=capture.json()['id']
    response=client.post(f'/api/experiments/{eid}/restore');assert response.status_code==200
    restored=Topology.model_validate(response.json())
    assert fingerprint(restored)==before
    assert client.get(f'/api/topologies/{restored.id}').status_code==200
    assert assess(restored,check_environment=False)['valid']
    assert {n.id for n in restored.nodes}.isdisjoint({n.id for n in t.nodes})
    assert all(u.core_id in {c.id for c in restored.nodes if c.kind=='epc'} for u in restored.nodes if u.kind in ('ue','enb'))
    assert all(u.radio_environment.startswith(u.core_id+':') for u in restored.nodes if u.kind=='ue')
    assert client.get('/api/experiments').json() and client.get('/api/simulations').json()['state']=='stopped'


@pytest.mark.asyncio
async def test_stopped_ue_retains_last_ip_without_current_attachment(tmp_path):
    from app.storage import Store
    from app.engine import Engine
    import time
    store=Store(tmp_path/'db');engine=Engine(store)
    t=build_template('two-ues')
    ues=[n for n in t.nodes if n.kind=='ue']
    engine.active={'id':'history-fixture','state':'running','started_epoch':time.time(),
                   'components':{'ue':'running','ue-2':'running'},'topology':t.model_dump(),
                   'ue_states':{u.id:{'component_id':u.id,'role':'ue' if i==0 else 'ue-2',
                                     'nas_attached':False,'rrc_connected':False,'ip':None}
                                for i,u in enumerate(ues)}}
    class Runner:
        returncode=0
        @property
        def stdout(self):
            async def lines():
                for role,ip in [('ue','172.16.0.2'),('ue-2','172.16.0.3')]:
                    yield json.dumps({'type':'attachment','component':role,'attached':True,'ip':ip}).encode()
                    yield json.dumps({'type':'attachment','component':role,'attached':False,'ip':None}).encode()
            return lines()
        async def wait(self):return self.returncode
    try:
        await engine.consume(Runner())
        status=engine.status()
        assert status['state']=='stopped'
        for u,ip in zip(ues,['172.16.0.2','172.16.0.3']):
            evidence=status['ues'][u.id]
            assert evidence['last_ip']==ip
            assert evidence['ip'] is None
            assert not evidence['nas_attached']
            assert evidence['serving_cell'] is None
            assert store.get('run','history-fixture')['ue_states'][u.id]['last_ip']==ip
    finally:
        store.engine.dispose()


@pytest.mark.parametrize('count',[3,5,12])
def test_design_sized_ue_allocation_preserves_independent_identities(tmp_path,count):
    from app.network import diagnose
    import importlib.util
    from app.integration_contract import project
    t=build_template('two-ues')
    source=next(n for n in t.nodes if n.kind=='ue')
    t.nodes=[n for n in t.nodes if n.kind!='ue']
    t.links=[l for l in t.links if l.kind!='zmq']
    for i in range(count):
        t.nodes.append(source.model_copy(deep=True,update={'id':f'ue-{i+1}',
            'config':source.config.model_copy(update={'imsi':f'00101{123456780+i:010d}'})}))
    assert not diagnose(t)['unsupported']
    mapping=plan(t)
    assert len(mapping['ues'])==count
    assert len({p['ue_tx'] for p in mapping['ues']} | {p['broker_dl'] for p in mapping['ues']})==2*count
    assert len({p['tun_namespace'] for p in mapping['ues']})==count
    directory=tmp_path/'run';generate(REPO,directory,t)
    spec=importlib.util.spec_from_file_location('scaled_runner',Path(__file__).resolve().parents[1]/'orchestrator/runner.py')
    module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
    runner=module.MatrixRunner(directory)
    assert len(runner.ue_roles)==count
    assert f'ue-{count}' in runner.manifest
    rows=(directory/'epc-user_db.csv').read_text().splitlines()
    assert len(rows)==count and len({r.split(',')[2] for r in rows})==count
    run={'id':'fixture','state':'stopped','topology':t.model_dump(),'configuration_hashes':{f'ue-{count}.conf':'digest'},'components':{f'ue-{count}':'stopped'}}
    event=project('run',run)
    assert event['data']['components']=={f'ue-{count}':'stopped'}
    assert event['data']['applied_topology']['configuration_identity']['radio_file_sha256'][f'ue-{count}.conf']=='digest'
