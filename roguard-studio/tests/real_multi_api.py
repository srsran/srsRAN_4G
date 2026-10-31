"""Opt-in actual native multi-cell API acceptance against an isolated Studio."""
import json
import os
from pathlib import Path
import time
import httpx
from real_multi_probe import topology,STUDIO

def main():
    t=topology();enbs=[n for n in t.nodes if n.kind=='enb']
    url=os.environ.get('STUDIO_TEST_API','http://127.0.0.1:8000')
    c=httpx.Client(base_url=url,headers={'Origin':'http://127.0.0.1:3000'},timeout=60,trust_env=False)
    c.post('/api/session').raise_for_status()
    def call(method,path,body=None):
        r=c.request(method,'/api'+path,json=body);r.raise_for_status();return r.json()
    run=None
    try:
        call('PUT',f'/topologies/{t.id}',t.model_dump())
        validation=call('POST',f'/topologies/{t.id}/validate')
        assert validation['runnable'],validation
        preview=call('POST','/configurations/preview',t.model_dump())
        assert len(preview['rf_plan']['paths'])==sum(len(n.cells) for n in enbs)
        call('POST',f'/simulations/{t.id}/start')
        deadline=time.monotonic()+110
        while time.monotonic()<deadline:
            status=call('GET','/simulations')
            if status['state']=='failed':raise AssertionError(status['run']['error'])
            if status['state']=='running' and status['attached'] and status['serving_cell'] and status['run'].get('broker',{}).get('data',{}).get('downlink_samples',0)>0:break
            time.sleep(.4)
        else:raise AssertionError('No attached multi-cell native API evidence')
        run=status['run'];sample=status['run']['broker']['data']
        pids={n.id:status['node_components'][n.id]['pid'] for n in enbs}
        assert len(set(pids.values()))==len(enbs)
        assert status['components']['broker']['pid']
        assert all(path['input_samples']>0 and path['output_samples']>0 and path['input_energy']>0 for path in sample['paths'])
        if float(os.environ.get('STUDIO_PROBE_ATTENUATION','20'))>0:
            assert status['serving_cell']['pci']==[1,6,11][int(os.environ.get('STUDIO_PROBE_TARGET','0'))]
        time.sleep(3)
        call('POST','/simulations/stop')
        deadline=time.monotonic()+8
        while time.monotonic()<deadline:
            metrics=call('GET',f'/metrics?run_id={run["id"]}')
            seen={m.get('component_id') for m in metrics if m['component']=='enb'}
            if seen=={n.id for n in enbs}:break
            time.sleep(.2)
        assert seen=={n.id for n in enbs},seen
        assert call('GET','/simulations')['serving_cell'] is None
        name='multicell-'+str(len(enbs))+'enb-'+str(len(sample['paths']))+'cell'
        result={'run_id':run['id'],'topology_id':t.id,'component_pids':pids,'broker_pid':status['components']['broker']['pid'],
                'serving_cell':status['serving_cell'],'broker':sample,'rf_plan':run['rf_plan'],
                'per_enb_native_metrics':sorted(seen),'actual_native_attachment':True,'handover_verified':False,
                'owned_shutdown':'stopped','directory':run['directory']}
        (STUDIO/'docs'/f'{name}-acceptance.json').write_text(json.dumps(result,indent=2)+'\n')
        print(json.dumps(result,indent=2))
    finally:
        call('POST','/simulations/stop');c.close()
if __name__=='__main__':main()
