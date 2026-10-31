"""Real HTTP/SSE transport and crash-recovery test; source inputs are protocol fixtures.

Run with ROGUARD_REPO pointing to the independent product repository. This test
never launches radio processes and does not count as native LTE demonstration.
"""
import asyncio
import importlib
import os
from pathlib import Path
import socket
import sys

import pytest
import uvicorn

from app.main import create_app


@pytest.mark.asyncio
async def test_real_transport_replays_after_bridge_disconnect(tmp_path, monkeypatch):
    repo = Path(os.environ.get('ROGUARD_REPO', Path(__file__).resolve().parents[3] / 'rogue-bs-detection'))
    if not (repo / 'backend/simulation_bridge').is_dir():
        pytest.skip('Independent Roguard backend unavailable; set ROGUARD_REPO')
    sys.path.insert(0, str(repo / 'backend'))
    Bridge = importlib.import_module('simulation_bridge.bridge').Bridge
    Settings = importlib.import_module('simulation_bridge.bridge').Settings
    BridgeStore = importlib.import_module('simulation_bridge.storage').Store
    token = 'transport-fixture-' + 't' * 40
    monkeypatch.setenv('STUDIO_INTEGRATION_TOKEN', token)
    app = create_app(tmp_path / 'studio')
    listener = socket.socket()
    listener.bind(('127.0.0.1', 0))
    port = listener.getsockname()[1]
    listener.listen()
    server = uvicorn.Server(uvicorn.Config(app, log_level='error', lifespan='on'))
    server_task = asyncio.create_task(server.serve(sockets=[listener]))
    bridge_task = None
    store = None
    async def wait_for(predicate):
        async with asyncio.timeout(10):
            while not predicate():
                await asyncio.sleep(.03)
    try:
        await wait_for(lambda: server.started)
        for i in range(3):
            app.state.engine.audit('lifecycle', f'protocol fixture {i}, not native radio')
        path = tmp_path / 'consumer/bridge.sqlite3'
        store = BridgeStore(path)
        settings = Settings(f'http://127.0.0.1:{port}', token)
        bridge = Bridge(store, settings)
        bridge_task = asyncio.create_task(bridge.run())
        await wait_for(lambda: store.cursor() == 3 and bridge.phase == 'connected')
        source = store.source_id()
        bridge_task.cancel()
        await asyncio.gather(bridge_task, return_exceptions=True)
        store.close()
        store = None
        # A native-process fixture arrives with no consumer and no browser connected.
        app.state.engine.audit('lifecycle', 'offline protocol fixture')
        app.state.engine.audit('scenario', 'synthetic protocol fixture', scenario={
            'id': 'fixture', 'scenario_id': 'pci-collision', 'state': 'active', 'ground_truth': 'Suspicious',
        })
        store = BridgeStore(path)
        bridge = Bridge(store, settings)
        bridge_task = asyncio.create_task(bridge.run())
        await wait_for(lambda: store.cursor() == 5 and bridge.phase == 'connected')
        assert store.source_id() == source
        assert store.db.execute('SELECT COUNT(*) FROM raw_events').fetchone()[0] == 5
        assert store.db.execute('SELECT COUNT(*) FROM raw_snapshots').fetchone()[0] == 2
        assert store.dashboard()['events'][0]['source_type'] == 'synthetic-experiment'
        assert store.dashboard()['topology_changes'] == []
        # Exclusive replay retains the original journal IDs across consumer restart.
        expected = app.state.store.integration_events(0)
        assert [row[0] for row in store.db.execute('SELECT event_id FROM raw_events ORDER BY sequence')] == [e['event_id'] for e in expected]
    finally:
        if bridge_task:
            bridge_task.cancel()
            await asyncio.gather(bridge_task, return_exceptions=True)
        if store:
            store.close()
        server.should_exit = True
        await server_task
        listener.close()
