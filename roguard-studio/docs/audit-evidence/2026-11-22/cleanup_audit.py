from pathlib import Path
import psutil,signal,time,json,httpx
root=Path('/tmp/studio-audit-20261122');studio=root/'work/srsRAN_4G/roguard-studio';out={'api_stop':{},'owned_service_pids':[]}
for port in (18180,18181):
 with httpx.Client(base_url=f'http://127.0.0.1:{port}',trust_env=False,headers={'Origin':'http://127.0.0.1:3000'},timeout=50) as c:
  c.post('/api/session').raise_for_status();r=c.post('/api/simulations/stop');r.raise_for_status();s=r.json();out['api_stop'][str(port)]={'state':s['state'],'attached':s['attached'],'serving_cell':s['serving_cell']}
  assert s['state'] in ('stopped','failed') and not s['attached'] and s['serving_cell'] is None
services=[];descendants=[]
for p in psutil.process_iter(['pid','cmdline','cwd']):
 args=p.info['cmdline'] or []
 own_api=any(str(root/name) in args for name in ('start_api.py','start_native_api.py'))
 own_front=(p.info['cwd']==str(studio/'frontend') and '--port' in args and '13180' in args and 'dev' in args)
 if own_api or own_front:
  services.append(p);descendants.extend(p.children(recursive=True));out['owned_service_pids'].append(p.pid)
for p in services:
 try:p.send_signal(signal.SIGINT)
 except psutil.NoSuchProcess:pass
_,alive=psutil.wait_procs(services,timeout=12)
for p in alive:
 try:p.terminate()
 except psutil.NoSuchProcess:pass
psutil.wait_procs(alive,timeout=5)
for p in descendants:
 try:
  if p.is_running() and p.status()!=psutil.STATUS_ZOMBIE:p.terminate()
 except psutil.NoSuchProcess:pass
psutil.wait_procs(descendants,timeout=5)
left=[]
for p in psutil.process_iter(['pid','exe','status']):
 exe=p.info['exe'] or ''
 if exe.startswith(str(root/'work/srsRAN_4G')) and p.info['status']!=psutil.STATUS_ZOMBIE:left.append({'pid':p.pid,'executable':exe})
out['remaining_audit_native_processes']=left
out['audit_services_gone']=all(not p.is_running() or p.status()==psutil.STATUS_ZOMBIE for p in services)
out['user_port_8100_still_listening']=any(c.status=='LISTEN' and c.laddr and c.laddr.port==8100 for c in psutil.net_connections(kind='tcp'))
(root/'cleanup-result.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2));assert not left and out['audit_services_gone']
