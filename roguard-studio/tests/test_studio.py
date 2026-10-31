import configparser
import io
import json
from pathlib import Path
import zipfile
import pytest
from pydantic import ValidationError
from fastapi.testclient import TestClient
from app.models import Config, Topology, Link
from app.configuration import generate, sanitized_config
from app.discovery import REPO, validate
from app.main import create_app
from app.storage import Store, now
from app.engine import Engine


def test_topology_rejects_bad_links_and_duplicate_ids(baseline):
    d=baseline.model_dump();d['links'][0]['target']='ue'
    with pytest.raises(ValidationError):Topology.model_validate(d)
    d=baseline.model_dump();d['nodes'][1]['id']='epc'
    with pytest.raises(ValidationError):Topology.model_validate(d)

@pytest.mark.parametrize('kwargs',[{'rf_driver':'uhd'},{'device_id':'x\n[log]'},{'mme_addr':'8.8.8.8'},{'mcc':'310'},{'tx_port':80},{'tx_port':2001,'rx_port':2001},{'earfcn':float('nan')},{'extra_args':'--anything'}])
def test_untrusted_configuration_rejected(kwargs):
    with pytest.raises(ValidationError):Config(**kwargs)

def test_validate_mismatch_and_multi_cell(baseline):
    assert validate(baseline)['valid']
    baseline.nodes[-1].config.rx_port=3000
    assert any('reciprocal' in e for e in validate(baseline)['errors'])
    baseline.nodes.append(baseline.nodes[1].model_copy(update={'id':'second-cell'}))
    assert any('process IDs must be unique' in e for e in validate(baseline)['adapter_blockers'])

def test_generator_private_and_redacted(tmp_path,baseline):
    original=(REPO/'srsue/ue.conf.example').read_bytes()
    directory=tmp_path/'run';hashes=generate(REPO,directory,baseline)
    assert len(hashes)==7
    assert original==(REPO/'srsue/ue.conf.example').read_bytes()
    c=configparser.ConfigParser();c.read(directory/'ue.conf')
    assert c['gw']['netns']=='rgstudio-ue'
    assert c['usim']['k'] not in sanitized_config(directory/'ue.conf')
    assert c['usim']['opc'] not in sanitized_config(directory/'ue.conf')
    assert (directory/'ue.conf').stat().st_mode & 0o077==0
    assert 'authentication material omitted' in sanitized_config(directory/'user_db.csv')
    assert 'dl_earfcn = 3350;' in (directory/'rr.conf').read_text()
    with pytest.raises(FileExistsError):generate(REPO,directory,baseline)

def test_session_origin_and_host_guards(tmp_path):
    with TestClient(create_app(tmp_path)) as c:
        assert c.get('/api/system').status_code==401
        assert c.post('/api/session').status_code==403
        assert c.post('/api/session',headers={'Origin':'https://malicious.example'}).status_code==403
        assert c.post('/api/session',headers={'Origin':'http://127.0.0.1:3000'}).status_code==200
        assert c.get('/api/system',headers={'Host':'rebind.example'}).status_code==403
        assert c.post('/api/simulations/stop',headers={'Origin':'https://malicious.example'}).status_code==403
        with pytest.raises(Exception):
            with c.websocket_connect('/api/ws',headers={'Origin':'https://malicious.example'}):pass

def test_revision_validation_and_no_fake_baseline(client,baseline):
    saved=client.put('/api/topologies/default',json=baseline.model_dump())
    assert saved.status_code==200
    assert saved.json()['revision']==1
    assert client.put('/api/topologies/default',json=baseline.model_dump()).status_code==409
    assert client.post('/api/topologies/default/validate').json()['valid']
    assert client.post('/api/baselines/default',json={'reason':'Designed baseline'}).status_code==409
    assert client.get('/api/simulations').json()['state']=='stopped'
    assert client.post('/api/simulations/stop').status_code==200

def test_synthetic_event_and_experiment_survive_restart(tmp_path,baseline):
    directory=tmp_path/'state'
    with TestClient(create_app(directory),headers={'Origin':'http://127.0.0.1:3000'}) as c:
        c.post('/api/session');c.put('/api/topologies/default',json=baseline.model_dump())
        event=c.post('/api/scenarios',json={'scenario_id':'unexpected-plmn','target':'enb','topology_id':'default','parameters':{'mnc':'02'}})
        assert event.status_code==200
        assert event.json()['source_type']=='synthetic'
        assert event.json()['ground_truth']=='Suspicious'
        assert c.get('/api/metrics').json()==[]
        assert c.get('/api/simulations').json()['observed'] is None
        assert c.post('/api/scenarios/'+event.json()['id']+'/stop').json()['state']=='stopped'
        e=c.post('/api/experiments',json={'name':'Test experiment','category':'Suspicious','topology_id':'default'})
        assert e.status_code==201;eid=e.json()['id']
    with TestClient(create_app(directory),headers={'Origin':'http://127.0.0.1:3000'}) as c:
        c.post('/api/session')
        assert c.get('/api/experiments').json()[0]['id']==eid
        data=c.get(f'/api/experiments/{eid}/export?format=json').json()
        assert data['metrics']==[] and data['applied_run'] is None
        assert data['model_prediction'] is None and data['analyst_decision'] is None
        assert data['scenario_events'][0]['execution_method']=='SIMULATED EVENT'
        archive=zipfile.ZipFile(io.BytesIO(c.get(f'/api/experiments/{eid}/export').content))
        assert {'experiment.json','metrics.csv'}<=set(archive.namelist())
        assert c.get(f'/api/experiments/{eid}/export?format=csv').text.startswith('timestamp_utc,run_id')

def test_metrics_partial_rows_nan_and_real_baseline_comparison(tmp_path,baseline):
    store=Store(tmp_path/'db');engine=Engine(store)
    engine.active={'id':'run-test','directory':str(tmp_path),'topology':baseline.model_dump(),'state':'running','started_epoch':0}
    path=tmp_path/'ue_metrics.csv'
    path.write_text('time;cc;earfcn;pci;rsrp;dl_snr;is_attached\n1000;0;3350;1;-80;nan;1\n2000;0;3350')
    engine.ingest();metrics=store.list('metric')
    assert len(metrics)==1 and metrics[0]['values']['dl_snr'] is None
    assert engine.active['observed']['parameters']=={'pci':1,'earfcn':3350}
    engine.ingest();assert len(store.list('metric'))==1
    with path.open('a') as f:f.write(';2;-81;20;1\n')
    engine.ingest();assert len(store.list('metric'))==2
    assert engine.active['observed']['parameters']['pci']==2
    assert len(store.list('topology-event'))==2
    assert all(e['comparison_status']=='baseline_missing' for e in store.list('topology-event'))
    store.engine.dispose()

def test_analyst_approval_audited(client,baseline):
    client.put('/api/topologies/default',json=baseline.model_dump())
    engine=client.app.state.engine
    engine.active={'id':'fixture','state':'running','topology':baseline.model_dump(),'directory':'/tmp/nonexistent-studio-fixture','started_epoch':0,'attached':True}
    engine.observe({'pci':1,'earfcn':3350},'unit-test fixture (not live LTE)')
    event=client.get('/api/topology-events').json()[0]
    response=client.post(f'/api/topology-events/{event["id"]}/approve',json={'reason':'Authorized laboratory change'})
    assert response.status_code==200 and response.json()['classification']=='Legitimate Change'
    assert client.get('/api/baselines').json()[0]['parameters']=={'pci':1,'earfcn':3350}
    assert any(e['type']=='baseline' for e in client.get('/api/events').json())

def test_pre_authorized_change_still_requires_approval(client,baseline):
    client.put('/api/topologies/default',json=baseline.model_dump())
    engine=client.app.state.engine
    engine.active={'id':'fixture','state':'running','topology':baseline.model_dump(),'directory':'/tmp/nonexistent-studio-fixture','started_epoch':0,'attached':True}
    client.app.state.store.put('baseline','default',{'parameters':{'pci':1,'earfcn':3350},'topology_id':'default'})
    assert client.post('/api/baselines/default/authorize',json={'pci':2,'earfcn':3350,'reason':'Authorized temporary serving cell'}).status_code==200
    engine.observe({'pci':2,'earfcn':3350},'unit-test fixture')
    e=client.get('/api/topology-events').json()[0]
    assert e['previously_authorized'] and e['classification'] is None
    assert e['analyst_decision'] is None
    assert client.get('/api/baselines').json()[0]['parameters']['pci']==1
    engine.observe({'pci':3,'earfcn':3350},'unit-test fixture')
    assert any(e['classification'] is None and e['comparison_status']=='analysis_required' for e in client.get('/api/topology-events').json())

def test_layout_does_not_require_radio_restart(baseline):
    a=baseline.model_dump();b=baseline.model_dump()
    b['revision']=100;b['name']='Renamed project';b['nodes'][0]['position']['x']=100
    assert Engine.runtime_signature(a)==Engine.runtime_signature(b)
    b['nodes'][1]['cells'][0]['config']['pci']=3
    assert Engine.runtime_signature(a)!=Engine.runtime_signature(b)

def test_restore_snapshot_preserves_evidence_and_remaps_design(client,baseline):
    client.put('/api/topologies/default',json=baseline.model_dump())
    eid=client.post('/api/experiments',json={'name':'Restore test','topology_id':'default'}).json()['id']
    restored=client.post(f'/api/experiments/{eid}/restore')
    assert restored.status_code==200
    t=restored.json();assert t['id']!='default' and t['revision']==1
    assert not ({n['id'] for n in t['nodes']} & {n.id for n in baseline.nodes})
    assert client.get(f'/api/experiments/{eid}/export?format=json').json()['topology']['id']=='default'
    assert client.get('/api/simulations').json()['state']=='stopped'
