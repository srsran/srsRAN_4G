"""Opt-in native broker-crash cleanup, unrelated-process preservation and restart."""
import json
import os
from pathlib import Path
import subprocess
import time
import httpx
import psutil
from real_multi_probe import topology,STUDIO

def main():
    client=httpx.Client(base_url=os.environ.get('STUDIO_TEST_API','http://127.0.0.1:8000'),headers={'Origin':'http://127.0.0.1:3000'},timeout=60,trust_env=False)
    client.post('/api/session').raise_for_status()
    def call(method,path,body=None):
        response=client.request(method,'/api'+path,json=body);response.raise_for_status();return response.json()
    def wait(predicate,timeout=110):
        end=time.monotonic()+timeout
        while time.monotonic()<end:
            status=call('GET','/simulations')
            if predicate(status):return status
            time.sleep(.25)
        raise AssertionError('Native lifecycle predicate timed out')
    unrelated=subprocess.Popen(['sleep','180'])
    try:
        t=topology();call('PUT',f'/topologies/{t.id}',t.model_dump());call('POST',f'/simulations/{t.id}/start')
        status=wait(lambda s:s['state']=='running' and s['attached'] and s['serving_cell'])
        broker=psutil.Process(status['components']['broker']['pid'])
        assert broker.exe()==str(STUDIO/'var/tools/rf_broker')
        assert status['run']['supervisor_pid'] in [p.pid for p in broker.parents()]
        owned=[(p.pid,p.create_time()) for p in psutil.Process(status['run']['supervisor_pid']).children(recursive=True)]
        broker.kill()
        ended=wait(lambda s:s['state']=='failed' and s['run'].get('finished_at'),45)
        assert not ended['attached'] and ended['serving_cell'] is None
        assert unrelated.poll() is None
        for pid,created in owned:
            try:p=psutil.Process(pid)
            except psutil.NoSuchProcess:continue
            assert p.create_time()!=created or p.status()==psutil.STATUS_ZOMBIE,(pid,p.name())
        call('POST',f'/simulations/{t.id}/start')
        restarted=wait(lambda s:s['state']=='running' and s['attached'] and s['serving_cell'])
        call('POST','/simulations/stop')
        result={'failed_run_id':status['run']['id'],'restart_run_id':restarted['run']['id'],
                'broker_crash_detected':True,'owned_descendants_cleaned':True,'unrelated_process_preserved':True,
                'restart_attached':True,'final_state':call('GET','/simulations')['state']}
        (STUDIO/'docs/multicell-failure-acceptance.json').write_text(json.dumps(result,indent=2)+'\n')
        print(json.dumps(result,indent=2))
    finally:
        call('POST','/simulations/stop');client.close();unrelated.terminate();unrelated.wait()
if __name__=='__main__':main()
