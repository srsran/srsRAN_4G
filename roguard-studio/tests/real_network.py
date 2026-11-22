"""Opt-in native acceptance of exact network profiles; no radio/process mocks.

Run only on a Linux laboratory host with unprivileged user/network namespaces.
Private runs/credentials remain in Studio var; --output writes public evidence.
"""
import argparse
import asyncio
import json
import time
from pathlib import Path
import sys
from uuid import uuid4
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'backend'))
from app.discovery import STUDIO,installation,BINS
from app.storage import Store,now
from app.engine import Engine
from app.templates import build_template
from app.compatibility import fingerprint,broker_tool
from app.network import implementation_hash
import psutil

async def check(template_id, *, kill_ue=False, stop_core=False):
    topology=build_template(template_id)
    store=Store(STUDIO/'var'/('network-acceptance-'+str(uuid4())))
    store.put('topology',topology.id,topology.model_dump())
    engine=Engine(store);snapshots=[];previous_cpu={};role_peaks={};failure=None
    result={'test_id':'network-'+template_id,'template_id':template_id,'date':now(),
            'source_commit':installation()['commit'],'native_version':installation()['version'],'native_binary_sha256':installation()['native_binary_sha256'],
            'implementation_sha256':implementation_hash(),'broker_binary_sha256':broker_tool()['sha256'],'fingerprint':fingerprint(topology),
            'topology':topology.model_dump(),'result':'failed','native_verified':False,'warnings':[]}
    try:
        await engine.start(topology)
        deadline=time.monotonic()+170
        while time.monotonic()<deadline:
            engine.ingest();status=engine.status()
            if status['state']=='failed':raise AssertionError(engine.active.get('error','native startup failed'))
            if engine.proc and engine.proc.returncode is None:
                owned=psutil.Process(engine.proc.pid).children(recursive=True)
                sampled_at=time.monotonic();rss=0;cpu=0
                for process in owned:
                    try:
                        memory=process.memory_info().rss;times=process.cpu_times();identity=(process.pid,process.create_time());total=times.user+times.system
                    except (psutil.NoSuchProcess,psutil.AccessDenied):continue
                    rss+=memory;prior=previous_cpu.get(identity);usage=100*max(0,total-prior[0])/(sampled_at-prior[1]) if prior else 0
                    previous_cpu[identity]=(total,sampled_at);cpu+=usage
                    role=next((name for name,component in status['components'].items() if component.get('pid')==process.pid),None)
                    if role:
                        peak=role_peaks.setdefault(role,{'peak_rss_bytes':0,'peak_cpu_percent':0})
                        peak['peak_rss_bytes']=max(peak['peak_rss_bytes'],memory);peak['peak_cpu_percent']=max(peak['peak_cpu_percent'],usage)
                snapshots.append({'rss_bytes':rss,'cpu_percent':cpu})
            if status['state']=='running' and all(u['nas_attached'] and u['serving_cell'] for u in status['ues'].values()):break
            await asyncio.sleep(.5)
        else:raise AssertionError('Not all independent UEs produced NAS-success and fresh native serving evidence')
        await asyncio.sleep(4);engine.ingest();status=engine.status()
        assert all(u['nas_attached'] and u['serving_cell'] for u in status['ues'].values())
        pids={n.id:status['node_components'][n.id]['pid'] for n in topology.nodes}
        assert len(set(pids.values()))==len(topology.nodes),'Native component PIDs collided'
        for broker in engine.active.get('brokers',{}).values():
            assert all(p['input_samples']>0 and p['output_samples']>0 for p in broker['data'].get('paths',[]))
            assert all(p['uplink_input_samples']>0 and p['downlink_output_samples']>0 for p in broker['data'].get('ues',[]))
        result.update(run_id=engine.active['id'],rf_adapter=(engine.active.get('rf_plan') or {}).get('adapter','direct'),
                      configuration_hashes=engine.active['configuration_hashes'],component_pids=pids,
                      ue_evidence=status['ues'],brokers=engine.active.get('brokers',{}),
                      readiness=status['components'],warnings=engine.active['validation']['warnings'])
        owned=[(p.pid,p.create_time()) for p in psutil.Process(engine.proc.pid).children(recursive=True)]
        if kill_ue:
            second=next(u for u in status['ues'].values() if u['role']=='ue-2')
            child=psutil.Process(pids[second['component_id']]);assert child.exe()==str(BINS['ue'])
            child.kill();await asyncio.wait_for(engine.task,45)
            status=engine.status();assert status['state']=='failed' and not any(u['nas_attached'] for u in status['ues'].values())
            assert status['components']['ue-2']['state']=='failed'
            assert status['components']['ue']['state']=='stopped'
            result['failure_policy_verified']={'failed_role':'ue-2','healthy_role_final_state':status['components']['ue']['state'],'experiment_state':status['state']}
        if stop_core:
            core=[n for n in topology.nodes if n.kind=='epc'][1]
            await engine.stop_core(core.id)
            deadline=time.monotonic()+40
            while engine.active.get('environments',{}).get(core.id)!='stopped' and time.monotonic()<deadline:
                engine.ingest();await asyncio.sleep(.25)
            status=engine.status();assert engine.active['environments'][core.id]=='stopped'
            alive=[u for u in status['ues'].values() if u['core_id']!=core.id]
            stopped=[u for u in status['ues'].values() if u['core_id']==core.id]
            assert alive and all(u['nas_attached'] for u in alive)
            assert stopped and not any(u['nas_attached'] for u in stopped)
            result['independent_core_stop_verified']=True
        await engine.stop();engine.ingest()
        for pid,created in owned:
            try:p=psutil.Process(pid)
            except psutil.NoSuchProcess:continue
            assert p.create_time()!=created or p.status()==psutil.STATUS_ZOMBIE,'Owned native process survived shutdown'
        metrics=store.list('metric')
        expected={n.id for n in topology.nodes if n.kind=='ue'}
        assert {m.get('component_id') for m in metrics if m['component']=='ue'}==expected
        result['persisted_metric_components']=sorted({m.get('component_id') or m['component'] for m in metrics})
        result['resources']={'logical_cpus':psutil.cpu_count(),'peak_owned_rss_bytes':max((s['rss_bytes'] for s in snapshots),default=0),
                             'peak_sampled_cpu_percent':max((s['cpu_percent'] for s in snapshots),default=0),'cpu_unit':'percent summed across owned processes; may exceed 100',
                             'per_process':role_peaks,'measurement':'RSS sum and user+system CPU-time deltas sampled during startup/attachment; short workload, not sustained capacity benchmark'}
        result.update(result='passed',native_verified=True,finished_at=now())
    except Exception as exc:
        failure=exc;result.update(error=str(exc),run_id=(engine.active or {}).get('id'),recent_logs=list(engine.logs)[-20:])
    finally:
        await engine.stop();store.engine.dispose()
    return result

async def main():
    parser=argparse.ArgumentParser();parser.add_argument('--templates',nargs='+',default=['single-cell']);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--kill-ue-2',action='store_true');parser.add_argument('--stop-second-core',action='store_true');args=parser.parse_args()
    results=[]
    for template in args.templates:
        row=await check(template,kill_ue=args.kill_ue_2,stop_core=args.stop_second_core)
        results.append(row);args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps({'schema_version':'studio.native-network-evidence.v1','profiles':results},indent=2)+'\n')
        print(json.dumps({k:row.get(k) for k in ('test_id','result','run_id','error','resources')}),flush=True)
    if any(r['result']!='passed' for r in results):raise SystemExit(1)

if __name__=='__main__':asyncio.run(main())
