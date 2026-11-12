"""Configure RoGuard's existing bridge from Studio's authenticated local UI."""
import asyncio
import os
import shlex
from urllib.parse import urlsplit

import httpx
from fastapi import HTTPException
from pydantic import BaseModel, Field, SecretStr, field_validator


def local_origin(value):
    value = value.strip()
    if '://' not in value:
        value = 'http://' + value
    parsed = urlsplit(value)
    try:
        port = parsed.port
    except ValueError:
        raise ValueError('Invalid local service port')
    if (parsed.scheme != 'http' or parsed.hostname not in ('localhost', '127.0.0.1') or
            parsed.username is not None or parsed.password is not None or parsed.query or parsed.fragment or
            parsed.path not in ('', '/', '/app', '/app/', '/app/simulation') or port == 0):
        raise ValueError('Use a localhost HTTP service URL without credentials or query parameters')
    return 'http://' + parsed.netloc


class ConnectionInput(BaseModel):
    dashboard_url: str = Field(min_length=1, max_length=2048)
    studio_url: str = Field(min_length=1, max_length=2048)
    credential: SecretStr = SecretStr('')

    @field_validator('dashboard_url', 'studio_url')
    @classmethod
    def validate_url(cls, value):
        return local_origin(value)


def local_analyst_credential(studio):
    if 'ROGUARD_ANALYST_TOKEN' in os.environ:
        return os.environ['ROGUARD_ANALYST_TOKEN']
    path = studio.parents[1]/'rogue-bs-detection/backend/.env'
    if not path.is_file():
        return ''
    token = ''
    for line in path.read_text().splitlines():
        key, separator, raw = line.strip().removeprefix('export ').partition('=')
        if separator and key.strip() == 'ROGUARD_ANALYST_TOKEN':
            token = ' '.join(shlex.split(raw, comments=True))
    return token


def saved_source_token(directory):
    path = directory/'integration-token'
    return path.read_text().strip() if path.exists() else ''


def persist_source_token(directory, token):
    descriptor = os.open(directory/'integration-token', os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
    with os.fdopen(descriptor, 'w') as stream:
        stream.write(token)


async def read_connection(client, url):
    response = await client.get(url + '/api/simulation/connection')
    if response.status_code == 404:
        raise HTTPException(409, 'This RoGuard service needs the frontend connection update. Restart its dashboard and bridge after updating them.')
    if response.status_code != 200:
        raise HTTPException(409, 'RoGuard dashboard or bridge is unavailable. Start both services and check the dashboard URL.')
    state = response.json()
    if not isinstance(state, dict) or not isinstance(state.get('bridge'), dict) or 'studio_url' not in state:
        raise HTTPException(409, 'This URL is not a RoGuard dashboard. Enter the RoGuard address rather than the Studio address.')
    return state


def connected(state, source_id, studio_url):
    return (state.get('source_id') == source_id and
            state.get('studio_url', '').rstrip('/') == studio_url.rstrip('/') and
            state['bridge'].get('connection') == 'connected')


async def configure_connection(body, token, source_id):
    try:
        async with httpx.AsyncClient(timeout=10, trust_env=False, follow_redirects=False) as client:
            state = await read_connection(client, body.dashboard_url)
            if connected(state, source_id, body.studio_url):
                return state
            credential = body.credential.get_secret_value()
            if not credential:
                raise HTTPException(409, 'Local RoGuard credentials were not found. Enter the RoGuard analyst credential in the connection form.')
            response = await client.post(body.dashboard_url + '/api/simulation/connection',
                headers={'Origin': body.dashboard_url, 'Authorization': 'Bearer ' + credential},
                json={'studio_url': body.studio_url, 'studio_token': token, 'expected_source_id': source_id})
            if response.status_code == 401:
                raise HTTPException(409, 'RoGuard rejected the connection credential. Check the analyst credential.')
            if response.status_code != 200:
                raise HTTPException(409, 'RoGuard could not validate this Studio API. Check the API port and existing source enrollment. Existing evidence was preserved.')
            for _ in range(30):
                await asyncio.sleep(.5)
                state = await read_connection(client, body.dashboard_url)
                if connected(state, source_id, body.studio_url):
                    return state
            raise HTTPException(409, 'Connection settings saved. RoGuard is still connecting or replaying evidence; wait briefly, then try Connect again.')
    except (httpx.HTTPError, ValueError):
        raise HTTPException(409, 'Cannot reach the RoGuard connection API. Check the dashboard URL and start its simulation bridge.')
