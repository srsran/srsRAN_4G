"""Native candidate-adapter probe. No bypass of the public Start API."""
import asyncio
import csv
import json
import os
from pathlib import Path
import sys
import time
from uuid import uuid4
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'backend'))
from app.models import Topology,Component,Config,Cell,CellConfig
from app.configuration import generate_multi
from app.discovery import REPO,STUDIO

def topology():
    count=int(os.environ.get('STUDIO_PROBE_CELLS','2'))
    intra=os.environ.get('STUDIO_PROBE_INTRA')=='1'
    mixed=os.environ.get('STUDIO_PROBE_LAYOUT')=='mixed'
    attenuation=float(os.environ.get('STUDIO_PROBE_ATTENUATION','20'))
    target=int(os.environ.get('STUDIO_PROBE_TARGET','0'))
    enbs=[]
    for i in range(count):
        cell=Cell(id=f'probe-cell-{i}',config=CellConfig(cell_id=i+1,pci=[1,6,11][i],rf_port=i if intra else (1 if mixed and i==1 else 0)),rf_attenuation_db=0 if i==target else attenuation)
        if (intra or mixed and i==1) and enbs:enbs[0].cells.append(cell)
        else:enbs.append(Component(id=f'probe-enb-{i}',kind='enb',name=f'Probe eNodeB {i}',config=Config(enb_id=411+i,bind_addr=f'127.0.1.{i+1}',device_id=f'enb{i}'),cells=[cell]))
    t=Topology(id='probe-'+str(uuid4()),nodes=[Component(id='probe-core',kind='epc',name='Probe core'),*enbs,
        Component(id='probe-ue',kind='ue',name='Probe UE',config=Config(tx_port=2001,rx_port=2000,device_id='ue'))])
    return t

async def main():
    t=topology();enbs=[n for n in t.nodes if n.kind=='enb'];count=sum(len(n.cells) for n in enbs)
    attenuation=float(os.environ.get('STUDIO_PROBE_ATTENUATION','20'));target=int(os.environ.get('STUDIO_PROBE_TARGET','0'))
    directory=STUDIO/'var/runs'/t.id
    generate_multi(REPO,directory,t)
    env={**os.environ,'ROGUARD_PARENT_NETNS':os.readlink('/proc/self/ns/net')}
    p=await asyncio.create_subprocess_exec('unshare','--user','--map-root-user','--net','--mount','--fork','--kill-child=SIGTERM',
        sys.executable,str(STUDIO/'orchestrator/runner.py'),str(directory),stdin=asyncio.subprocess.PIPE,stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.STDOUT,env=env,start_new_session=True)
    events=[];running=False
    async def consume():
        nonlocal running
        with (directory/'probe-events.jsonl').open('w') as f:
            async for raw in p.stdout:
                line=raw.decode(errors='replace');f.write(line);f.flush()
                try:e=json.loads(line)
                except ValueError:continue
                events.append(e)
                if e['type']=='phase' and e['state']=='running':running=True
                if e['type'] in ('failure','phase'):print(e,flush=True)
    task=asyncio.create_task(consume())
    try:
        end=time.monotonic()+110
        while time.monotonic()<end and p.returncode is None and not running:await asyncio.sleep(.25)
        if not running:raise AssertionError('No native multi-cell attachment; inspect '+str(directory))
        await asyncio.sleep(8)
        rows=list(csv.DictReader((directory/'ue_metrics.csv').read_text().splitlines(),delimiter=';'))
        attached=[r for r in rows if r.get('is_attached') and float(r['is_attached'])==1]
        assert attached,'No attached native UE CSV row'
        observed=int(attached[-1]['pci'])
        if attenuation>0:assert observed==[1,6,11][target],(observed,target)
        samples=[e['data'] for e in events if e['type']=='broker' and e['data']['event']=='samples'][-1]
        assert all(v['input_samples']>0 and v['output_samples']>0 and v['input_energy']>0 for v in samples['paths']),samples
        result={'directory':str(directory),'run_id':t.id,'enbs':len(enbs),'cells':count,'pci':observed,'earfcn':int(attached[-1]['earfcn']),
                'attached_rows':len(attached),'broker':samples,'native_multi_attachment':True,'handover_verified':False}
        (directory/'probe-result.json').write_text(json.dumps(result,indent=2));print(json.dumps(result,indent=2),flush=True)
    finally:
        if p.returncode is None:p.stdin.write(b'stop\n');await p.stdin.drain()
        try:await asyncio.wait_for(task,45)
        except asyncio.TimeoutError:
            import signal
            os.killpg(p.pid,signal.SIGTERM);await asyncio.wait_for(task,10)
        await p.wait();print('Exit:',p.returncode,flush=True)
if __name__=='__main__':asyncio.run(main())
