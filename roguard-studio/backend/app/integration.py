"""Authenticated read-only SSE and paginated replay over the durable journal."""
import asyncio
import json
import re
import time

from fastapi import APIRouter, HTTPException, Query, Request
from fastapi.responses import StreamingResponse

from .integration_contract import IntegrationEvent, SCHEMA
from .storage import now


def router():
    routes = APIRouter(prefix='/integration/v1', tags=['read-only integration'])

    def cursor(store, value):
        if value == '0':
            return 0
        match = re.fullmatch(r'([^:]+):(0|[1-9][0-9]{0,18})', value)
        if not match:
            raise HTTPException(400, 'Cursor must be 0 or source_id:sequence')
        if match[1] != store.source_id:
            raise HTTPException(409, 'Source identity changed; explicit bridge re-enrollment is required')
        sequence = int(match[2])
        if sequence > store.integration_head():
            raise HTTPException(409, 'Cursor is ahead of source history; database may have been restored')
        return sequence

    @routes.get('/snapshot')
    def snapshot(request: Request):
        return request.app.state.store.integration_snapshot()

    @routes.get('/schema')
    def schema():
        return IntegrationEvent.model_json_schema()

    @routes.get('/events')
    def events(request: Request, after: str = '0', limit: int = Query(200, ge=1, le=1000)):
        store = request.app.state.store
        sequence = cursor(store, after)
        items = store.integration_events(sequence, limit)
        return {'schema_version': SCHEMA, 'source_id': store.source_id, 'events': items,
                'next_cursor': items[-1]['event_id'] if items else f'{store.source_id}:{sequence}'}

    @routes.get('/stream')
    async def stream(request: Request, after: str = '0'):
        store = request.app.state.store
        sequence = cursor(store, request.headers.get('last-event-id') or after)

        async def generate():
            nonlocal sequence
            heartbeat = 0
            while not await request.is_disconnected():
                # The DB is the queue. Slow readers cannot drop source evidence.
                items = store.integration_events(sequence)
                for item in items:
                    yield f"id: {item['event_id']}\nevent: evidence\ndata: {json.dumps(item, allow_nan=False)}\n\n"
                    sequence = item['sequence']
                if time.monotonic() - heartbeat >= 2:
                    data = {'schema_version': SCHEMA, 'source_id': store.source_id, 'timestamp': now(), 'watermark': store.integration_head()}
                    yield 'event: heartbeat\ndata: ' + json.dumps(data) + '\n\n'
                    heartbeat = time.monotonic()
                await asyncio.sleep(.25)

        return StreamingResponse(generate(), media_type='text/event-stream', headers={
            'Cache-Control': 'no-store', 'X-Accel-Buffering': 'no',
        })

    return routes
