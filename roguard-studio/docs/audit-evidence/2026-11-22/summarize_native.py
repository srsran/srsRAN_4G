from pathlib import Path
import json,csv,math,hashlib
root=Path('/tmp/studio-audit-20261122');studio=root/'work/srsRAN_4G/roguard-studio'
rows=[]
for directory in sorted((studio/'var/runs').iterdir()):
 if not directory.is_dir():continue
 parsed=[]
 p=directory/'ue_metrics.csv'
 if p.exists():
  try:parsed=[r for r in csv.DictReader([l for l in p.read_text().splitlines() if not l.startswith('#')],delimiter=';') if r.get('is_attached')=='1.0']
  except (ValueError,KeyError):pass
 fields=('pci','earfcn','rsrp','dl_snr','dl_brate','ul_brate','dl_bler','ul_bler','pci_neigh','rsrp_neigh','cfo_neigh','proc_rmem_kB','sys_load')
 samples={k:sorted({r.get(k,'') for r in parsed})[:8] for k in fields}
 s1={p.name:('S1Setup procedure completed successfully' in p.read_text(errors='replace')) for p in directory.glob('enb*.log')}
 ue=(directory/'ue.log').read_text(errors='replace') if (directory/'ue.log').exists() else ''
 broker=[]
 if (directory/'broker.log').exists():
  for line in (directory/'broker.log').read_text().splitlines():
   try:d=json.loads(line)
   except ValueError:continue
   if d.get('event')=='samples':broker.append(d)
 rows.append({'run_id':directory.name,'directory':str(directory),'native_nas_success':'Network attach successful. IP:' in ue,'attached_csv_rows':len(parsed),'native_values':samples,'enb_s1_success':s1,'last_broker':broker[-1] if broker else None})
(root/'native-evidence-summary.json').write_text(json.dumps(rows,indent=2)+'\n')
print(json.dumps([{'run_id':r['run_id'],'nas':r['native_nas_success'],'csv_rows':r['attached_csv_rows'],'s1':r['enb_s1_success']} for r in rows],indent=2))
