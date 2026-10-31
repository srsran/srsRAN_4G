"""Machine-interface security, durable replay and native telemetry projection tests."""
import json
from concurrent.futures import ThreadPoolExecutor

from fastapi.testclient import TestClient
import pytest
from sqlalchemy import select

from app.main import create_app
from app.integration_contract import IntegrationEvent
from app.storage import Store, IntegrationRecord
from app.engine import Engine

TOKEN = 'integration-test-token-' + 'x' * 32


def test_machine_token_is_read_only_and_separate(tmp_path, monkeypatch):
    monkeypatch.setenv('STUDIO_INTEGRATION_TOKEN', TOKEN)
    with TestClient(create_app(tmp_path)) as client:
        headers = {'Authorization': 'Bearer ' + TOKEN}
        assert client.get('/integration/v1/snapshot').status_code == 401
        assert client.get('/integration/v1/snapshot', headers=headers).status_code == 200
        assert client.get('/integration/v1/snapshot', headers={**headers, 'Origin': 'http://localhost:3000'}).status_code == 403
        for path in ('/api/session', '/api/simulations/stop', '/api/experiments', '/integration/v1/snapshot'):
            assert client.post(path, headers=headers).status_code in (403, 405)
        assert client.get('/api/system', headers=headers).status_code == 403
        with pytest.raises(Exception):
            with client.websocket_connect('/api/ws', headers={**headers, 'Origin': 'http://localhost:3000'}):
                pass
        client.app.state.engine.audit('lifecycle', 'private arbitrary log; k=SECRET')
        page = client.get('/integration/v1/events', headers=headers).json()
        assert len(page['events']) == 1
        assert 'SECRET' not in json.dumps(page)
        event = IntegrationEvent.model_validate(page['events'][0])
        assert event.source_type == 'native-process'
        assert client.get('/integration/v1/events', params={'after': page['next_cursor']}, headers=headers).json()['events'] == []
        assert client.get('/integration/v1/events', params={'after': 'wrong-source:1'}, headers=headers).status_code == 409
        assert client.get('/integration/v1/events', params={'after': event.source_id + ':99'}, headers=headers).status_code == 409
        assert client.get('/integration/v1/events?after=invalid', headers=headers).status_code == 400


def test_disabled_machine_interface(tmp_path, monkeypatch):
    monkeypatch.delenv('STUDIO_INTEGRATION_TOKEN', raising=False)
    with TestClient(create_app(tmp_path)) as client:
        assert client.get('/integration/v1/snapshot').status_code == 503


def test_atomic_journal_and_private_field_allowlist(tmp_path, baseline):
    store = Store(tmp_path / 'state')
    engine = Engine(store)
    run = {'id': 'test-run', 'directory': str(tmp_path), 'topology': baseline.model_dump(),
           'state': 'running', 'started_at': '2026-10-30T00:00:00Z', 'started_epoch': 0,
           'supervisor_pid': 123, 'software': {'private': 'SECRET'}, 'configuration_hashes': {'key': 'SECRET'}}
    engine.active = run
    engine.persist()
    (tmp_path / 'ue_metrics.csv').write_text('time;cc;earfcn;pci;rsrp;dl_snr;is_attached;private\n1000;0;3350;1;-80;nan;1;12345\n')
    engine.ingest()
    events = store.integration_events(0, 1000)
    assert len(events) >= 3
    for event in events:
        IntegrationEvent.model_validate(event)
    metric = next(e for e in events if e['kind'] == 'metric')
    assert metric['data']['units']['time'] == 'ms'
    assert metric['data']['units']['rsrp'] == 'dBm'
    assert metric['data']['values']['dl_snr'] is None
    serialized = json.dumps(events)
    assert all(secret not in serialized for secret in ('imsi', 'supervisor_pid', 'directory', 'SECRET', '12345'))
    assert metric['topology_id'] == baseline.id
    snapshot = store.integration_snapshot()
    assert snapshot['watermark'] == events[-1]['sequence']
    assert snapshot['runtime']['data']['observed']['parameters']['pci'] == 1
    assert snapshot['metrics'][0]['event_id'] == metric['event_id']
    before = store.integration_head()
    engine.persist()
    assert store.integration_head() == before  # Unchanged runtime is not re-emitted.
    # Journal failure rolls back the source mutation as well.
    original = store._journal
    def fail(*args, **kwargs):
        raise RuntimeError('injected transaction failure')
    store._journal = fail
    with pytest.raises(RuntimeError):
        store.put('event', 'failed-write', {'id': 'bad', 'type': 'lifecycle'})
    assert store.get('event', 'failed-write') is None
    store._journal = original
    identity = store.source_id
    store.engine.dispose()
    reopened = Store(tmp_path / 'state')
    assert reopened.source_id == identity
    assert reopened.integration_head() == before
    assert reopened.integration_events(metric['sequence'] - 1, 1)[0] == metric
    reopened.engine.dispose()


def test_concurrent_journal_is_contiguous(tmp_path):
    store = Store(tmp_path)
    def write(i):
        store.put('event', str(i), {'id': str(i), 'timestamp': '2026-10-30T00:00:00Z', 'type': 'lifecycle'})
    with ThreadPoolExecutor(max_workers=4) as pool:
        list(pool.map(write, range(30)))
    assert [e['sequence'] for e in store.integration_events(0)] == list(range(1, 31))
    store.engine.dispose()


def test_bootstraps_legacy_observation_with_nested_run_identity(tmp_path, baseline):
    import sqlite3
    path = tmp_path / 'studio.sqlite3'
    db = sqlite3.connect(path)
    db.execute('CREATE TABLE records (key VARCHAR PRIMARY KEY, kind VARCHAR NOT NULL, value JSON NOT NULL)')
    run = {'id': 'legacy-run', 'state': 'stopped', 'topology': baseline.model_dump(),
           'started_at': '2026-10-29T00:00:00Z', 'attached': False}
    legacy = {'id': 'legacy-observation', 'type': 'topology-observation',
              'timestamp': '2026-10-29T00:00:02Z', 'message': 'legacy audit',
              'observed': {'parameters': {'pci': 1, 'earfcn': 3350},
                           'timestamp': '2026-10-29T00:00:02Z', 'source': 'legacy unit fixture', 'run_id': run['id']}}
    for kind, key, value in [('run', run['id'], run), ('event', legacy['id'], legacy)]:
        db.execute('INSERT INTO records VALUES (?,?,?)', (kind+':'+key, kind, json.dumps(value)))
    db.commit()
    db.close()
    store = Store(tmp_path)
    items = store.integration_events(0)
    observed = next(item for item in items if item['kind'] == 'observation')
    assert observed['topology_id'] == baseline.id and observed['run_id'] == run['id']
    identity, head = store.source_id, store.integration_head()
    store.engine.dispose()
    store = Store(tmp_path)
    assert store.source_id == identity and store.integration_head() == head
    store.engine.dispose()
