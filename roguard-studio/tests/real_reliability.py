"""Opt-in isolated LTE crash, recovery, EOF cleanup and host-preservation checks."""
import asyncio
import json
import os
from pathlib import Path
import signal
import sys
import time
import psutil
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'backend'))
from app.storage import Store
from app.discovery import STUDIO
from app.engine import Engine
from real_smoke import baseline

async def wait_state(engine,states,timeout=120):
    end=time.monotonic()+timeout
    while time.monotonic()<end:
        engine.ingest()
        if engine.status()['state'] in states:return engine.status()
        await asyncio.sleep(.25)
    raise AssertionError(f'Timed out waiting for {states}: {engine.status()["state"]}')

async def main():
    # Leave an unrelated process alive to ensure cleanup never kills by name or a broad process pattern.
    unrelated=await asyncio.create_subprocess_exec('sleep','180',start_new_session=True)
    store=Store(STUDIO/'var/reliability-test');engine=Engine(store)
    before={n for n in os.listdir('/sys/class/net')}
    results={}
    try:
        await engine.start(baseline())
        s=await wait_state(engine,{'running','failed'});assert s['state']=='running'
        await asyncio.sleep(2);engine.ingest();s=engine.status();assert s['observed']
        owned=psutil.Process(engine.proc.pid).children(recursive=True)
        ue=next(p for p in owned if p.name()=='srsue')
        ue.kill() # Kill only the verified owned child; this intentionally exercises crash detection.
        await wait_state(engine,{'failed'})
        await asyncio.wait_for(engine.task,30)
        assert not engine.status()['attached'] and engine.status()['state']=='failed'
        assert unrelated.returncode is None
        results['crash_detected_and_dependencies_cleaned']=True
        results['first_run_id']=engine.active['id']
        await engine.start(baseline())
        s=await wait_state(engine,{'running','failed'});assert s['state']=='running'
        results['restart_attached']=s['attached'];results['second_run_id']=engine.active['id']
        # Simulate controller loss by closing its private control pipe. Runner must shut down on EOF.
        engine.proc.stdin.close()
        await asyncio.wait_for(engine.task,40)
        assert engine.status()['state']=='stopped' and not engine.status()['attached']
        results['control_pipe_eof_cleanup']=True
        assert unrelated.returncode is None
        assert before=={n for n in os.listdir('/sys/class/net')}
        results['host_interfaces_unchanged']=True;results['unrelated_process_preserved']=True
        results['timestamp']=__import__('datetime').datetime.now(__import__('datetime').timezone.utc).isoformat()
        (STUDIO/'docs/reliability-results.json').write_text(json.dumps(results,indent=2)+'\n')
        print(json.dumps(results,indent=2),flush=True)
    finally:
        await engine.stop()
        if unrelated.returncode is None:unrelated.terminate();await unrelated.wait()
        store.engine.dispose()
if __name__=='__main__':asyncio.run(main())
