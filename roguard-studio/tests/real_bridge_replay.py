"""Replay actual native evidence from a test Studio through the existing Roguard bridge.

Run after real_api.py against an isolated Studio backend. No external application
files are modified; the consumer database lives in a temporary directory.
"""
import asyncio
import json
import os
from pathlib import Path
import sys
import tempfile
import httpx

STUDIO = Path(__file__).resolve().parents[1]
REPO = STUDIO.parent
sys.path.insert(0, str(Path(os.environ.get('ROGUARD_REPO', REPO.parent/'rogue-bs-detection'))/'backend'))
from simulation_bridge.bridge import Bridge, Settings
from simulation_bridge.storage import Store

async def main():
    url = os.environ.get('STUDIO_TEST_API', 'http://127.0.0.1:8000')
    token = os.environ['STUDIO_INTEGRATION_TOKEN']
    evidence = json.loads((STUDIO/'docs'/os.environ.get('STUDIO_BRIDGE_EVIDENCE','workbench-api-acceptance-results.json')).read_text())
    expected_pci=evidence.get('serving_cell',{}).get('pci',2)
    run_id = evidence['run_id']
    async with httpx.AsyncClient(base_url=url, headers={'Authorization':'Bearer '+token}, trust_env=False) as client:
        snapshot = (await client.get('/integration/v1/snapshot')).json()
    with tempfile.TemporaryDirectory(prefix='studio-native-bridge-') as tmp:
        store = Store(Path(tmp)/'bridge.sqlite3')
        bridge = Bridge(store, Settings(url, token))
        task = asyncio.create_task(bridge.run())
        try:
            async with asyncio.timeout(30):
                while store.cursor() < snapshot['watermark'] or bridge.phase != 'connected':
                    await asyncio.sleep(.1)
            rows = [json.loads(r[0]) for r in store.db.execute('SELECT payload FROM raw_events ORDER BY sequence')]
            metric = next(e for e in rows if e.get('run_id')==run_id and e['kind']=='metric'
                          and e['data']['component']=='ue' and e['data']['values'].get('is_attached')==1
                          and e['data']['values'].get('pci')==expected_pci)
            normalized = store.db.execute('SELECT payload FROM normalized_events WHERE event_id=?',(metric['event_id'],)).fetchone()
            assert normalized and metric['source_type']=='native-measurement'
            assert store.db.execute('SELECT COUNT(*) FROM rejections').fetchone()[0] == 0
            result = {'run_id':run_id,'source_id':store.source_id(), 'event_id':metric['event_id'],
                      'sequence':metric['sequence'], 'measurement':metric['data']['values'],
                      'provenance':metric['data']['provenance'], 'cursor':store.cursor(),
                      'authenticated_sse':bridge.stream_open, 'native_evidence_reached_existing_bridge':True,
                      'consumer_state':'temporary isolated replay; customer deployment unchanged'}
            (STUDIO/'docs'/os.environ.get('STUDIO_BRIDGE_OUTPUT','workbench-bridge-validation.json')).write_text(json.dumps(result,indent=2)+'\n')
            print(json.dumps(result,indent=2))
        finally:
            task.cancel()
            await asyncio.gather(task,return_exceptions=True)
            store.close()

if __name__=='__main__': asyncio.run(main())
