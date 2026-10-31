import os
import re
import shutil
import socket
import subprocess
from functools import lru_cache
from pathlib import Path
import psutil

STUDIO=Path(__file__).resolve().parents[2]
REPO=STUDIO.parent
BINS={k:REPO/f'build/{v}/src/{v}' for k,v in {'epc':'srsepc','enb':'srsenb','ue':'srsue'}.items()}

@lru_cache(maxsize=1)
def installation():
    cache=REPO/'build/CMakeCache.txt'
    txt=cache.read_text() if cache.exists() else ''
    plugin=list((REPO/'build/lib/src/phy/rf').glob('libsrsran_rf_zmq.so*'))
    configs=[]
    for root in [Path.home()/'.config/srsran',Path('/etc/srsran'),Path('/usr/local/etc/srsran')]:
        for name in ['enb.conf','ue.conf','epc.conf','rr.conf','sib.conf','rb.conf','user_db.csv']:
            if (root/name).is_file(): configs.append(str(root/name))
    try:
        probe=subprocess.run(['unshare','--user','--map-root-user','--net','--mount','--fork','true'],capture_output=True,text=True,timeout=4)
        isolation=probe.returncode==0
        reason=probe.stderr.strip()[:300]
    except (OSError,subprocess.TimeoutExpired) as exc: isolation,reason=False,str(exc)
    try:
        commit=subprocess.run(['git','rev-parse','HEAD'],cwd=REPO,capture_output=True,text=True,timeout=2).stdout.strip()
    except (OSError,subprocess.TimeoutExpired): commit='unknown'
    try:
        version_output=subprocess.run([str(BINS['enb']),'--version'],capture_output=True,text=True,timeout=4).stdout
        match=re.search(r'Version ([\d.]+)',version_output)
        version=match[1] if match else 'unknown'
    except (OSError,subprocess.TimeoutExpired):version='unknown'
    return {'repository':str(REPO),'version':version,'commit':commit,'binaries':{k:{'path':str(v),'exists':v.is_file(),'executable':os.access(v,os.X_OK)} for k,v in BINS.items()},
            'zeromq_enabled':'ENABLE_ZEROMQ:BOOL=ON' in txt,'uhd_enabled':'ENABLE_UHD:BOOL=ON' in txt,'plugin':str(plugin[0]) if plugin else None,
            'tun_available':Path('/dev/net/tun').exists(),'isolation_available':isolation,'isolation_diagnostic':reason,
            'tools':{k:shutil.which(k) for k in ['unshare','ip','mount','stdbuf']},'existing_config_paths':configs,
            'execution_adapter':'isolated-user-network-mount-namespaces','multi_cell':'NATIVE — two or three co-channel cells across one to three eNodeBs, one EPC and UE; 50 PRBs; handover not enabled',
            'version_source':str(REPO/'build/CMakeCache.txt')}

def sockets():
    try:
        return {'available':True,'scope':'backend host network namespace; the isolated LTE namespace has separate sockets',
                'sockets':[{'pid':c.pid,'local':f'{c.laddr.ip}:{c.laddr.port}' if c.laddr else None,'remote':f'{c.raddr.ip}:{c.raddr.port}' if c.raddr else None,'status':c.status} for c in psutil.net_connections(kind='inet') if c.laddr]}
    except (psutil.Error,PermissionError,OSError) as exc: return {'available':False,'reason':str(exc),'sockets':[]}

def resources():
    m=psutil.virtual_memory()
    return {'cpu_percent':psutil.cpu_percent(),'memory_percent':m.percent,'memory_used_bytes':m.used,'source':'psutil (backend host)'}

def validate(t):
    problems=[]
    adapter_blockers=[]
    warnings=[]
    from .radio import is_multi,restrictions,plan,pci_diagnostics,MULTICELL_VERIFIED,BROKER
    multi=is_multi(t)
    real=[n for n in t.nodes if n.kind in ('epc','enb','ue','temporary','suspicious')]
    groups={k:[n for n in real if n.kind in (('enb','temporary','suspicious') if k=='enb' else (k,))] for k in ('epc','enb','ue')}
    for k,ns in groups.items():
        if not ns:
            problems.append(f'Add at least one {k.upper()} to complete the network design.')
        elif len(ns)>1 and (not multi or k!='enb'):
            adapter_blockers.append(f'Current execution adapter supports exactly one {k.upper()}; found {len(ns)}. Multi-process execution requires a verified I/Q broker and process adapter.')
    if all(len(ns)==1 for ns in groups.values()):
        p,e,u=[groups[k][0] for k in ('epc','enb','ue')]
        if not multi and (e.config.tx_port,e.config.rx_port)!=(u.config.rx_port,u.config.tx_port): problems.append('ZeroMQ endpoints must be reciprocal: eNB TX = UE RX, UE TX = eNB RX.')
        if len({(n.config.mcc,n.config.mnc) for n in (p,e,u)})!=1: problems.append('EPC, eNodeB and UE must use the same synthetic PLMN.')
        if not u.config.imsi.startswith(u.config.mcc+u.config.mnc): problems.append('UE IMSI must match its selected synthetic PLMN.')
        cell = e.cells[0].config if e.cells else None
        if e.cells and not any(c.config.dl_earfcn==u.config.earfcn for c in e.cells): problems.append('UE search EARFCN must match at least one configured cell.')
        if p.config.mme_addr!=e.config.mme_addr: problems.append('eNodeB MME address must match EPC.')
        if p.config.mme_addr==e.config.bind_addr: problems.append('EPC and eNodeB GTP-U bindings must differ.')
        if cell and p.config.tac!=cell.tac: problems.append('EPC and eNodeB TAC must match for this baseline.')
        if p.config.apn!=u.config.apn: problems.append('EPC and UE APN must match.')
        if e.config.n_prb==75: problems.append('75 PRBs need a different base sample rate. This verified generator is restricted to 23.04 Msps.')
        for n in real:
            if n.config.log_level!='info': adapter_blockers.append(f'{n.name}: this adapter requires info logging for lifecycle readiness evidence.')
        for n in t.nodes:
            if n.kind in ('namespace','sector','channel','zmq','event'): warnings.append(f'{n.name}: diagram annotation only. The fixed UE namespace and node radio fields control runtime.')
    for n in groups['enb']:
        if not n.cells:
            problems.append(f'{n.name}: add at least one owned cell to complete the eNodeB configuration.')
        elif len(n.cells)>1 and not multi:
            adapter_blockers.append(f'{n.name}: {len(n.cells)} owned cells are configured. The current Studio adapter supports one owned cell per eNodeB; the multi-cell I/Q broker is not integrated. The design can still be saved and previewed.')
        for cell in n.cells:
            c = cell.config
            if not multi and cell.rf_attenuation_db!=0: adapter_blockers.append(f'{cell.name}: set broker attenuation to 0 dB for direct single-cell mode.')
            if c.ho_active: adapter_blockers.append(f'{cell.name}: handover requires neighbor/report configuration and a verified RF broker; unavailable in current adapter.')
            if c.ul_earfcn is not None and c.ul_earfcn != c.dl_earfcn + 18000: problems.append(f'{cell.name}: Band 7 uplink must equal DL EARFCN + 18000.')
    identities=pci_diagnostics(t) if multi else {'errors':[],'changes':[]}
    if multi:
        problems.extend(identities['errors'])
        adapter_blockers.extend(message for message in restrictions(t) if message not in identities['errors'])
        cells=[c for n in t.nodes for c in n.cells]
        if len(cells)==3 and max(c.rf_attenuation_db for c in cells)-min(c.rf_attenuation_db for c in cells)<6:
            warnings.append('Equal-strength co-channel paths can prevent decoding. The three-cell selection baseline was verified with 20 dB attenuation on the other candidates; edit Broker path attenuation in the cell inspector.')
        if not MULTICELL_VERIFIED: adapter_blockers.append('Co-channel I/Q adapter is undergoing native verification.')
    env=installation()
    blockers=[]
    for k,v in env['binaries'].items():
        if not v['executable']: blockers.append(f'{k} binary missing or not executable')
    if not env['plugin'] or not env['zeromq_enabled']: blockers.append('Compiled ZeroMQ plugin unavailable')
    if not env['tun_available']: blockers.append('/dev/net/tun is unavailable in this execution environment')
    if not env['isolation_available']: blockers.append('Unprivileged user/network/mount namespace probe failed: '+env['isolation_diagnostic'])
    for k,v in env['tools'].items():
        if not v: blockers.append(f'Required Linux tool unavailable: {k}')
    if multi and not os.access(BROKER,os.X_OK): blockers.append('Build the native I/Q broker with roguard-studio/scripts/build-broker.py')
    return {'suggested_pci_changes':identities['changes'],'rf_plan':plan(t) if multi else None,'valid':not problems,'environment_ready':not blockers,'adapter_supported':not adapter_blockers,
            'runnable':not problems and not blockers and not adapter_blockers,
            'errors':problems,'runtime_blockers':blockers,'adapter_blockers':adapter_blockers,'warnings':warnings,
            'execution':'REAL — isolated co-channel I/Q broker' if multi else 'REAL — direct single-cell ZeroMQ',
            'multi_cell':env['multi_cell']}
