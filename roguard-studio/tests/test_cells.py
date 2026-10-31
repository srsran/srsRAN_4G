"""Owned-cell configuration, evidence correlation and immutable applied state."""
import copy
import pytest
from pydantic import ValidationError
from app.models import Cell, CellConfig, Component, Topology
from app.discovery import validate
from app.configuration import generate
from app.discovery import REPO
from app.engine import Engine
from app.integration_contract import project


def test_legacy_conversion_is_read_only_and_neutral():
    legacy={'id':'old-enb','kind':'suspicious','name':'Legacy name','config':{'pci':11,'earfcn':3350}}
    before=copy.deepcopy(legacy)
    node=Component.model_validate(legacy)
    assert legacy==before
    assert node.kind=='enb' and node.cells[0].config.pci==11
    assert 'suspicious' not in Component.model_json_schema()['properties']['kind']['enum']
    assert node.cells[0].id==Component.model_validate(legacy).cells[0].id
    explicit=Component(id='e',kind='enb',name='Empty',cells=[])
    assert explicit.cells==[]  # explicit deletion never silently recreates a cell


def test_cell_ownership_and_port_order(baseline):
    with pytest.raises(ValidationError): Component(kind='ue',name='UE',cells=[Cell()])
    d=baseline.model_dump()
    d['nodes'][1]['cells'].append({'id':'other','name':'Other','config':{'cell_id':2,'rf_port':0,'pci':6}})
    with pytest.raises(ValidationError,match='RF ports'): Topology.model_validate(d)
    d['nodes'][1]['cells'][1]['config']['rf_port']=1
    multi=Topology.model_validate(d)
    assert len(multi.nodes[1].cells)==2
    report=validate(multi)
    assert report['valid'] and report['adapter_supported']
    assert report['runnable']==report['environment_ready']
    assert report['errors']==[]
    d['nodes'][1]['cells'][1]['id']=d['nodes'][1]['cells'][0]['id']
    with pytest.raises(ValidationError,match='unique'): Topology.model_validate(d)


def test_generator_uses_owned_cell_and_refuses_truncation(tmp_path,baseline):
    baseline.nodes[1].cells[0].config=CellConfig(pci=11,root_seq_idx=204,ul_earfcn=21350)
    baseline.nodes[1].config.pci=99  # legacy field is not authoritative
    generate(REPO,tmp_path/'run',baseline)
    rr=(tmp_path/'run/rr.conf').read_text()
    assert 'pci = 11;' in rr and 'pci = 99;' not in rr
    assert 'root_seq_idx = 204;' in rr and 'ul_earfcn = 21350;' in rr
    baseline.nodes[1].cells.append(Cell(config=CellConfig(cell_id=2,rf_port=1,pci=6)))
    baseline.nodes[1].config.n_prb=25
    with pytest.raises(ValueError,match='50 PRBs'): generate(REPO,tmp_path/'bad',baseline)
    assert not (tmp_path/'bad').exists()


def test_preview_is_redacted_and_does_not_apply(client,baseline):
    payload=baseline.model_dump()
    payload['nodes'][1]['cells'][0]['config']['pci']=6
    r=client.post('/api/configurations/preview',json=payload)
    assert r.status_code==200
    data=r.json()
    assert 'pci = 6;' in data['files']['rr.conf']
    assert '[REDACTED]' in data['files']['ue.conf']
    assert 'user_db.csv' not in data['files']
    assert data['trace'][0]['model_path']=='nodes[1].cells[0].config'
    assert client.get('/api/simulations').json()['run'] is None
    assert client.get('/api/topologies').json()==[]


def test_applied_serving_cell_survives_design_edit_without_false_measurement(client,baseline):
    client.put('/api/topologies/default',json=baseline.model_dump())
    engine=client.app.state.engine
    engine.active={'id':'evidence-fixture','state':'running','topology':baseline.model_dump(),
                   'directory':'/tmp/no-studio-cell-fixture','started_epoch':0,'attached':True,
                   'observed':{'parameters':{'pci':1,'earfcn':3350},'timestamp':'2026-10-31T00:00:00Z'}}
    status=client.get('/api/simulations').json()
    assert status['serving_cell']['cell_id']==baseline.nodes[1].cells[0].id
    design=client.get('/api/topologies/default').json()
    design['nodes'][1]['cells'][0]['config']['pci']=6
    client.put('/api/topologies/default',json=design)
    status=client.get('/api/simulations').json()
    assert status['restart_required'] and status['serving_cell']['pci']==1
    assert status['run']['topology']['nodes'][1]['cells'][0]['config']['pci']==1
    engine.active['attached']=False
    assert client.get('/api/simulations').json()['serving_cell'] is None


def test_ambiguous_or_missing_serving_cell_is_unresolved(baseline):
    run={'state':'running','attached':True,'topology':baseline.model_dump(),
         'observed':{'parameters':{'pci':1,'earfcn':3350},'timestamp':'fixture'}}
    other=copy.deepcopy(run['topology']['nodes'][1]);other['id']='other';other['cells'][0]['id']='other-cell'
    run['topology']['nodes'].append(other)
    assert Engine.serving_cell(run) is None
    run['topology']['nodes'].pop()
    run['observed']['parameters']['pci']=503
    assert Engine.serving_cell(run) is None


def test_owned_cell_projection_is_configuration_only(baseline):
    baseline.nodes[1].cells[0].config.pci=11
    event=project('topology',baseline.model_dump())
    assert event['source_type']=='configuration'
    assert event['data']['nodes'][1]['configured_parameters']['pci']==11
    cell=event['data']['configured_cells'][0]
    assert cell['enb_id']=='enb' and 'not a native observation' in cell['provenance']
    assert 'classification' not in str(event)


def test_layout_and_mobility_never_change_runtime_signature(baseline):
    a=baseline.model_dump();b=copy.deepcopy(a)
    b['mobility']=[{'ue_id':'ue','speed':4,'duration':20,'start':{'x':0,'y':0},'end':{'x':80,'y':0}}]
    b['nodes'][1]['cells'][0]['name']='Renamed cell'
    assert Engine.runtime_signature(a)==Engine.runtime_signature(b)
    b['nodes'][1]['cells'][0]['config']['pci']=6
    assert Engine.runtime_signature(a)!=Engine.runtime_signature(b)


def test_runtime_extensions_fit_strict_existing_v1_consumer(baseline):
    from app.integration_contract import compatible_runtime
    run={'id':'fixture','topology':baseline.model_dump(),'state':'running','attached':True,
         'observed':{'parameters':{'pci':1,'earfcn':3350},'timestamp':'2026-10-31T00:00:00Z','source':'test fixture','run_id':'fixture'}}
    data=project('run',run)['data']
    assert set(data)=={'state','attached','components','observed','started_at','finished_at','applied_topology','configured_radio'}
    assert data['applied_topology']['configured_cells'][0]['enb_id']=='enb'
    assert data['applied_topology']['serving_cell']['pci']==1
    old={**data,'configured_cells':data['applied_topology']['configured_cells']}
    assert 'configured_cells' in old  # wire normalization never mutates retained input evidence
    assert compatible_runtime(old)==data


def test_unsupported_four_cell_start_cannot_silently_run_subset(client,baseline):
    baseline.nodes[1].cells += [Cell(config=CellConfig(cell_id=i+1,rf_port=i,pci=1+i*5)) for i in range(1,4)]
    assert client.put('/api/topologies/default',json=baseline.model_dump()).status_code==200
    response=client.post('/api/simulations/default/start')
    assert response.status_code==409
    assert not response.json()['detail']['runnable']
    assert any('two or three total cells' in e for e in response.json()['detail']['adapter_blockers'])
    assert client.app.state.engine.proc is None
    assert client.get('/api/simulations').json()['run'] is None


def test_native_connectivity_does_not_require_drawn_edges(baseline):
    baseline.links=[]
    assert validate(baseline)['valid']


def test_valid_two_cell_design_is_separate_from_host_and_adapter_readiness(baseline,monkeypatch):
    from app.discovery import installation
    env=installation().copy()
    # Environment diagnostic fixture only; no native execution is claimed here.
    env.update(tun_available=True,isolation_available=True,plugin='/fixture/zmq.so',zeromq_enabled=True,
               tools={'unshare':'/fixture/unshare'},binaries={'enb':{'executable':True}})
    monkeypatch.setattr('app.discovery.installation',lambda:env)
    baseline.nodes[1].cells.append(Cell(config=CellConfig(cell_id=2,rf_port=1,pci=6)))
    report=validate(baseline)
    assert report['valid'] and report['environment_ready']
    assert report['adapter_supported'] and report['runnable']
    assert report['errors']==[] and report['runtime_blockers']==[]
    assert report['adapter_blockers']==[]
    env['tun_available']=False
    report=validate(baseline)
    assert report['valid'] and not report['environment_ready']
    assert '/dev/net/tun' in report['runtime_blockers'][0]
    assert report['adapter_supported'] and not report['runnable']


def test_multicell_preview_generates_broker_files_without_launching(client,baseline):
    baseline.nodes[1].cells.append(Cell(config=CellConfig(cell_id=2,rf_port=1,pci=6)))
    response=client.post('/api/configurations/preview',json=baseline.model_dump())
    assert response.status_code==200
    data=response.json()
    assert data['validation']['valid'] and data['validation']['adapter_supported']
    assert 'rf_port = 0;' in data['files']['enb-rr.conf']
    assert 'rf_port = 1;' in data['files']['enb-rr.conf']
    assert client.get('/api/simulations').json()['run'] is None
    assert client.get('/api/topologies').json()==[]


def test_multiple_enb_files_and_collision_free_broker_allocation(tmp_path,baseline):
    import configparser
    from app.models import Config
    from app.radio import plan
    second=Component(id='second-enb',kind='enb',name='Second eNodeB',config=Config(enb_id=412,bind_addr='127.0.1.2'),
                     cells=[Cell(id='second-cell',config=CellConfig(pci=6),rf_attenuation_db=20)])
    baseline.nodes.append(second)
    mapping=plan(baseline)
    ports=[mapping['ue_tx'],mapping['broker_dl']]+[p[k] for p in mapping['paths'] for k in ('native_tx','broker_ul')]
    assert len(ports)==len(set(ports))
    directory=tmp_path/'multi'
    generate(REPO,directory,baseline)
    first=configparser.ConfigParser();first.read(directory/'enb.conf')
    other=configparser.ConfigParser();other.read(directory/'enb-2.conf')
    ue=configparser.ConfigParser();ue.read(directory/'ue.conf')
    assert first['enb']['s1c_bind_addr']=='127.0.1.1' and other['enb']['s1c_bind_addr']=='127.0.1.2'
    assert first['enb']['enb_id']=='411' and other['enb']['enb_id']=='412'
    assert '31000' in first['rf']['device_args'] and '31002' in other['rf']['device_args']
    assert '32000' in ue['rf']['device_args']
    assert mapping['paths'][1]['attenuation_db']==20
    assert 'pci = 6;' in (directory/'enb-2-rr.conf').read_text()


def test_native_process_status_ids_do_not_collide_with_roles(tmp_path,baseline):
    from app.storage import Store
    from app.models import Config
    from app.radio import plan
    baseline.nodes[1].id='enb-2';baseline.links=[]
    baseline.nodes.append(Component(id='B',kind='enb',name='B',config=Config(enb_id=412,bind_addr='127.0.1.2'),cells=[Cell(config=CellConfig(pci=6))]))
    store=Store(tmp_path/'status');engine=Engine(store)
    engine.active={'id':'fixture','topology':baseline.model_dump(),'state':'running','attached':False,
                   'components':{'enb':'ready','enb-2':'failed'},'started_epoch':0,'directory':str(tmp_path),
                   'rf_plan':plan(baseline)}
    status=engine.status()
    assert status['node_components']['enb-2']['state']=='ready'
    assert status['node_components']['B']['state']=='failed'
    assert status['components']['enb-2']['state']=='failed'
    store.engine.dispose()


def test_broker_attenuation_is_runtime_configuration(baseline):
    a=baseline.model_dump();b=copy.deepcopy(a)
    b['nodes'][1]['cells'][0]['rf_attenuation_db']=20
    assert Engine.runtime_signature(a)!=Engine.runtime_signature(b)


@pytest.mark.parametrize('pcis,expected', [([6,6],[1]),([6,9],[1]),([6,6,4],[11]),([6,6,6],[1,11])])
def test_pci_repairs_preserve_existing_cells_and_reserve_other_identities(baseline,pcis,expected):
    from app.radio import pci_diagnostics
    enb=baseline.nodes[1]
    enb.cells=[Cell(id=f'cell-{i}',name=f'Cell {i+1:02}',config=CellConfig(cell_id=i+1,rf_port=i,pci=pci)) for i,pci in enumerate(pcis)]
    before=baseline.model_dump()
    diagnostic=pci_diagnostics(baseline)
    assert baseline.model_dump()==before
    assert [change['to_pci'] for change in diagnostic['changes']]==expected
    assert all(change['cell_id']!='cell-0' for change in diagnostic['changes'])
    report=validate(baseline)
    assert not report['valid'] and not report['runnable']
    assert report['adapter_supported'] and report['adapter_blockers']==[]
    assert 'Cell 02' in report['errors'][0] and 'Cell 01' in report['errors'][0]
    for change in diagnostic['changes']:
        next(cell for cell in enb.cells if cell.id==change['cell_id']).config.pci=change['to_pci']
    assert validate(baseline)['valid']
    assert [(c.id,c.config.cell_id,c.config.rf_port) for c in enb.cells]==[(c['id'],c['config']['cell_id'],c['config']['rf_port']) for c in before['nodes'][1]['cells']]


def test_broker_uses_allocated_ports_after_original_enb_is_removed(baseline,tmp_path):
    enb=baseline.nodes[1]
    enb.config.enb_id=412
    enb.config.tx_port=2002;enb.config.rx_port=2003
    enb.cells[0].config.pci=6
    enb.cells.append(Cell(config=CellConfig(cell_id=2,rf_port=1,pci=1)))
    before=baseline.model_dump()
    assert validate(baseline)['valid']  # UE still has the old direct 2001/2000 pair
    generate(REPO,tmp_path/'broker',baseline)
    assert baseline.model_dump()==before
    assert '32000' in (tmp_path/'broker/ue.conf').read_text()
    assert '31002' in (tmp_path/'broker/enb.conf').read_text()
    enb.cells.pop()
    assert any('reciprocal' in error for error in validate(baseline)['errors'])


@pytest.mark.parametrize('pci',[6,9])
def test_generator_refuses_conflicting_pci_without_validation(baseline,tmp_path,pci):
    baseline.nodes[1].cells[0].config.pci=6
    baseline.nodes[1].cells.append(Cell(config=CellConfig(cell_id=2,rf_port=1,pci=pci)))
    with pytest.raises(ValueError,match='conflicts'):
        generate(REPO,tmp_path/'conflict',baseline)
    assert not (tmp_path/'conflict').exists()
