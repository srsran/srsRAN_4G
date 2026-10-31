"""Opt-in real API acceptance: apply a PCI change, authorize/review it, export and restore."""
import os
import configparser
import io
import json
from pathlib import Path
import sys
import time
from uuid import uuid4
import zipfile
import httpx
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'backend'))
from app.discovery import STUDIO
from real_smoke import baseline

def main():
    c=httpx.Client(base_url=os.environ.get('STUDIO_TEST_API','http://127.0.0.1:8000'),headers={'Origin':'http://127.0.0.1:3000'},timeout=60)
    c.post('/api/session').raise_for_status()
    def call(method,path,body=None):
        r=c.request(method,'/api'+path,json=body);r.raise_for_status();return r.json()
    def wait_running():
        deadline=time.monotonic()+120
        while time.monotonic()<deadline:
            s=call('GET','/simulations')
            if s['state']=='failed':raise AssertionError(s['run'].get('error'))
            if s['state']=='running' and s['attached'] and s['observed']:return s
            time.sleep(.3)
        raise AssertionError('No verified attached observation')
    t=baseline();t.id='acceptance-'+str(uuid4());t.name='Verified parameter-change experiment'
    for i,n in enumerate(t.nodes):n.position.x=80+i*350;n.position.y=180
    result={}
    try:
        saved=call('PUT',f'/topologies/{t.id}',t.model_dump())
        assert call('POST',f'/topologies/{t.id}/validate')['runnable']
        call('POST',f'/simulations/{t.id}/start');s=wait_running()
        assert s['observed']['parameters']['pci']==1
        call('POST',f'/baselines/{t.id}',{'reason':'Verified original serving-cell observation'})
        call('POST',f'/baselines/{t.id}/authorize',{'pci':2,'earfcn':3350,'reason':'Authorized laboratory PCI change'})
        saved['nodes'][1]['cells'][0]['config']['pci']=2
        saved=call('PUT',f'/topologies/{t.id}',saved)
        s=call('GET','/simulations')
        assert s['restart_required'] and s['observed']['parameters']['pci']==1
        assert s['run']['topology']['nodes'][1]['cells'][0]['config']['pci']==1
        original_rr=(Path(s['run']['directory'])/'rr.conf').read_bytes()
        preview=call('POST','/configurations/preview',saved)
        assert 'pci = 2;' in preview['files']['rr.conf'] and '+    pci = 2;' in preview['diffs']['rr.conf']
        assert original_rr==(Path(s['run']['directory'])/'rr.conf').read_bytes()
        result['preview_diff_did_not_mutate_applied_files']=True
        result['designed_change_did_not_mutate_runtime']=True
        call('POST',f'/simulations/{t.id}/restart');s=wait_running()
        assert s['observed']['parameters']['pci']==2
        event=next(e for e in call('GET','/topology-events') if e['run_id']==s['run']['id'])
        assert event['previously_authorized'] and event['classification'] is None and event['analyst_decision'] is None
        b=next(b for b in call('GET','/baselines') if b['topology_id']==t.id)
        assert b['parameters']['pci']==1
        call('POST',f'/topology-events/{event["id"]}/approve',{'reason':'Measured authorized PCI matches restart intent'})
        b=next(b for b in call('GET','/baselines') if b['topology_id']==t.id)
        assert b['parameters']['pci']==2
        result['actual_pci_change_observed_after_restart']=True
        result['pre_authorization_and_explicit_baseline_approval']=True
        call('POST','/scenarios',{'scenario_id':'unexpected-plmn','target':saved['nodes'][1]['id'],'topology_id':t.id,'parameters':{'synthetic_mnc':'02'}})
        assert call('GET','/simulations')['observed']['parameters']=={'pci':2,'earfcn':3350}
        e=call('POST','/experiments',{'name':'Verified live PCI change and synthetic scenario','category':'Legitimate Change','topology_id':t.id})
        r=c.get(f'/api/experiments/{e["id"]}/export');r.raise_for_status()
        archive=zipfile.ZipFile(io.BytesIO(r.content));data=json.loads(archive.read('experiment.json'))
        assert data['metrics'] and any(m['values'].get('pci')==2 for m in data['metrics'])
        assert data['scenario_events'][0]['execution_method']=='SIMULATED EVENT'
        config=configparser.ConfigParser();config.read(Path(s['run']['directory'])/'ue.conf')
        for secret in (config['usim']['k'],config['usim']['opc']):
            assert all(secret.lower().encode() not in archive.read(n).lower() for n in archive.namelist())
        result['actual_metrics_and_redacted_export']=True
        restored=call('POST',f'/experiments/{e["id"]}/restore')
        assert restored['id']!=t.id and call('GET','/simulations')['state']=='stopped'
        assert any(x['id']==e['id'] for x in call('GET','/experiments'))
        result['non_destructive_experiment_restore']=True
        result['run_id']=s['run']['id'];result['experiment_id']=e['id'];result['topology_id']=t.id
        result['timestamp']=__import__('datetime').datetime.now(__import__('datetime').timezone.utc).isoformat()
        (STUDIO/'docs/workbench-api-acceptance-results.json').write_text(json.dumps(result,indent=2)+'\n')
        print(json.dumps(result,indent=2))
    finally:
        call('POST','/simulations/stop');c.close()
if __name__=='__main__':main()
