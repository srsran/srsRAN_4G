import json

import httpx
import pytest
from pydantic import SecretStr

from app.roguard_connection import ConnectionInput, configure_connection, local_analyst_credential


@pytest.mark.parametrize('url', ['http://example.com', 'http://localhost:0',
                               'http://user:secret@localhost', 'http://localhost?token=x'])
def test_only_local_dashboard_urls(url):
    with pytest.raises(ValueError):
        ConnectionInput(dashboard_url=url, studio_url='http://localhost:8000')


def test_automatic_credential_uses_existing_local_file(tmp_path, monkeypatch):
    monkeypatch.delenv('ROGUARD_ANALYST_TOKEN', raising=False)
    studio = tmp_path/'srsRAN_4G/roguard-studio'
    path = tmp_path/'rogue-bs-detection/backend/.env'
    path.parent.mkdir(parents=True)
    path.write_text("ROGUARD_PORTAL_TOKEN=unrelated\nROGUARD_ANALYST_TOKEN='private-test-token'\n")
    assert local_analyst_credential(studio) == 'private-test-token'


@pytest.mark.asyncio
async def test_connect_updates_bridge_without_exposing_tokens(monkeypatch):
    requests = []
    configured = False

    def respond(request):
        nonlocal configured
        requests.append(request)
        if request.method == 'POST':
            assert request.headers['authorization'] == 'Bearer analyst-test-token'
            body = json.loads(request.content)
            assert body['studio_token'] == 'private-source-token'
            assert body['expected_source_id'] == 'test-source'
            configured = True
            return httpx.Response(200, json={'saved': True})
        return httpx.Response(200, json={
            'studio_url': 'http://localhost:8000' if configured else 'http://localhost:9999',
            'source_id': 'test-source', 'bridge': {'connection': 'connected' if configured else 'disconnected'}})

    real_client = httpx.AsyncClient
    monkeypatch.setattr('app.roguard_connection.httpx.AsyncClient',
                        lambda **kwargs: real_client(transport=httpx.MockTransport(respond), **kwargs))
    body = ConnectionInput(dashboard_url='localhost:3001', studio_url='localhost:8000',
                           credential=SecretStr('analyst-test-token'))
    state = await configure_connection(body, 'private-source-token', 'test-source')
    assert state['bridge']['connection'] == 'connected'
    assert 'private-source-token' not in json.dumps(state)
    assert len([row for row in requests if row.method == 'POST']) == 1


def test_ui_configuration_is_saved_and_tokens_stay_private(client, monkeypatch):
    async def configure(body, token, source_id):
        assert body.credential.get_secret_value() == 'local-analyst-token'
        assert len(token) >= 32

    monkeypatch.delenv('STUDIO_INTEGRATION_TOKEN', raising=False)
    monkeypatch.setattr('app.main.local_analyst_credential', lambda _: 'local-analyst-token')
    monkeypatch.setattr('app.main.configure_connection', configure)
    result = client.post('/api/roguard/connection', json={
        'dashboard_url': 'localhost:3001/app', 'studio_url': 'localhost:8000'})
    assert result.status_code == 200
    assert result.json()['connected']
    assert result.json()['dashboard_url'] == 'http://localhost:3001'
    assert 'credential' not in result.json()
    assert 'local-analyst-token' not in result.text


def test_failed_connection_does_not_start_simulation(client, monkeypatch):
    async def fail(*_):
        from fastapi import HTTPException
        raise HTTPException(409, 'RoGuard is unavailable')

    monkeypatch.setattr('app.main.configure_connection', fail)
    result = client.post('/api/roguard/connection', json={
        'dashboard_url': 'localhost:3001', 'studio_url': 'localhost:8000'})
    assert result.status_code == 409
    assert client.get('/api/simulations').json()['state'] == 'stopped'
