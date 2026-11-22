from pathlib import Path
import shutil,json,sys,hashlib,configparser,sqlite3
root=Path('/tmp/studio-audit-20261122');studio=root/'work/srsRAN_4G/roguard-studio';dest=Path('/mnt/disk-d/joe/Projects/srsRAN_4G/roguard-studio/docs/audit-evidence/2026-11-22');dest.mkdir(parents=True,exist_ok=True)
sys.path.insert(0,str(studio/'backend'));from app.configuration import sanitized_config
# Only explicit output classes; do not copy private credentials or temporary databases.
for pattern in ('*.log','*.xml','*.json','*.txt','actual-multienb-export.*'):
 for p in root.glob(pattern):
  if p.name=='private-integration-token':continue
  shutil.copy2(p,dest/p.name)
for name in ('audit_probes.py','summarize_native.py','start_api.py','start_native_api.py','cleanup_audit.py','package_evidence.py'):
 shutil.copy2(root/name,dest/name)
results=dest/'results';results.mkdir(exist_ok=True)
for name in ('workbench-api-acceptance-results.json','reliability-results.json','multicell-2enb-2cell-acceptance.json','multicell-3enb-3cell-acceptance.json','multicell-2enb-3cell-acceptance.json','multicell-failure-acceptance.json','workbench-bridge-validation.json','audit-three-enb-bridge.json'):
 shutil.copy2(studio/'docs'/name,results/name)
# Bind recorded hashes to evidence; private database is read only and stays in /tmp.
runs={}
for dbpath in (studio/'var').glob('**/studio.sqlite3'):
 with sqlite3.connect(f'file:{dbpath}?mode=ro',uri=True) as db:
  for raw, in db.execute("SELECT value FROM records WHERE kind='run'"):
   r=json.loads(raw);runs[r['id']]=r
needles=('MME Initialized','SP-GW Initialized','S1SetupResponse','S1Setup procedure completed','Found Cell:','Network attach successful. IP:','Network attach successful. APN:')
for directory in (studio/'var/runs').iterdir():
 if not directory.is_dir():continue
 out=dest/'native'/directory.name;out.mkdir(parents=True,exist_ok=True)
 for p in directory.glob('*metrics.csv'):shutil.copy2(p,out/p.name)
 for name in ('rf-plan.json','probe-result.json'):
  if (directory/name).exists():shutil.copy2(directory/name,out/name)
 if (directory/'broker.log').exists():shutil.copy2(directory/'broker.log',out/'broker-counters.jsonl')
 lines=[]
 for p in sorted(directory.glob('*.log')):
  for i,line in enumerate(p.read_text(errors='replace').splitlines(),1):
   if any(n in line for n in needles):lines.append(f'{p.name}:{i}: {line}')
 (out/'native-acceptance-lines.txt').write_text('\n'.join(lines)+'\n')
 confdir=out/'sanitized-configs';confdir.mkdir(exist_ok=True)
 for p in directory.glob('*.conf'):(confdir/p.name).write_text(sanitized_config(p))
 r=runs.get(directory.name,{})
 public={k:r[k] for k in ('id','state','started_at','finished_at','configuration_hashes','rf_plan','observed','attached','error') if k in r}
 public['current_configuration_hashes']={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in directory.glob('*.conf')}
 (out/'run-metadata.json').write_text(json.dumps(public,indent=2)+'\n')
# Retain only failure artifacts and responsive screenshots, not build/dependency artifacts.
shutil.copytree(studio/'frontend/test-results',dest/'browser-results',dirs_exist_ok=True)
for name in ('workbench-compact.png',):
 if (studio/'docs'/name).exists():shutil.copy2(studio/'docs'/name,dest/name)
# Verify no temporary bearer, UE K or OPc is present in any shared textual/archive payload.
secret_values=[(root/'private-integration-token').read_text()]
for p in (studio/'var/runs').glob('*/ue.conf'):
 c=configparser.ConfigParser();c.read(p)
 secret_values += [c['usim']['k'],c['usim']['opc']]
import zipfile,io
for p in dest.rglob('*'):
 if not p.is_file():continue
 data=p.read_bytes();members=[data]
 if p.suffix=='.zip':
  z=zipfile.ZipFile(io.BytesIO(data));members += [z.read(n) for n in z.namelist()]
 for member in members:
  assert not any(v.lower().encode() in member.lower() for v in secret_values),f'Secret in shared evidence: {p.name}'
files=[p for p in dest.rglob('*') if p.is_file()]
manifest={str(p.relative_to(dest)):hashlib.sha256(p.read_bytes()).hexdigest() for p in files}
(dest/'evidence-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
print('Evidence files:',len(files),'Bytes:',sum(p.stat().st_size for p in files),'secret scan: passed')
