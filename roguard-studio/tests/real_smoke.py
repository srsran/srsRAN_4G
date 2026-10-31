"""Opt-in REAL LTE lifecycle check. No mocked subprocesses or fabricated measurements."""
import asyncio
import json
from pathlib import Path
import sys
import time
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'backend'))
from app.models import Topology, Component, Config, Link
from app.storage import Store
from app.engine import Engine
from app.discovery import STUDIO,validate

def baseline():
    return Topology(nodes=[Component(id='epc-01',kind='epc',name='Smoke EPC'),Component(id='enb-01',kind='enb',name='Smoke eNB'),Component(id='ue-01',kind='ue',name='Smoke UE',config=Config(tx_port=2001,rx_port=2000,device_id='ue'))],links=[Link(id='s1',source='epc-01',target='enb-01',kind='s1'),Link(id='rf',source='enb-01',target='ue-01',kind='zmq')])
async def main():
    t=baseline();t.id='smoke-'+str(__import__('uuid').uuid4());t.name='Actual LTE smoke laboratory';
    for i,n in enumerate(t.nodes):n.position.x=80+i*350;n.position.y=180
    print(json.dumps(validate(t),indent=2),flush=True)
    store=Store(STUDIO/'var');engine=Engine(store);store.put('topology',t.id,t.model_dump())
    try:
        await engine.start(t)
        deadline=time.monotonic()+180
        while engine.active['state']=='starting' and time.monotonic()<deadline:
            engine.ingest();await asyncio.sleep(1)
        engine.ingest();print(json.dumps(engine.status(),indent=2),flush=True)
        if engine.active['state']!='running':
            for item in list(engine.logs)[-70:]:print(item['component']+': '+item['message'],flush=True)
            raise AssertionError('Actual LTE attachment not verified')
        await asyncio.sleep(5);engine.ingest()
        assert engine.status()['attached'],'UE attachment lost'
        assert engine.active.get('observed'),'No actual UE radio metrics'
        print('REAL LTE ATTACHMENT AND CSV OBSERVATION VERIFIED',flush=True)
    finally:
        await engine.stop();print('Final owned-process state: '+engine.status()['state'],flush=True);store.engine.dispose()
if __name__=='__main__':asyncio.run(main())
