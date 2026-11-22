"""Single admission boundary shared by validation, generation, templates and runtime.

Model validity is not execution support. Evidence is matched against an exact
radio/process fingerprint and installed native software, never a cell-count flag.
"""
import hashlib
import json
from pathlib import Path
from .core_adapters import CORES

REGISTRY_VERSION = 'studio.network-capabilities.v1'
EVIDENCE = Path(__file__).resolve().parents[2]/'docs/network-evidence/native-profiles.json'
# Non-power-of-two LTE FFT profile used by this checkout's phy_common.c.
PRB_RATES = {6: 1920000, 15: 3840000, 25: 5760000, 50: 11520000, 100: 23040000}
CHANNEL_MHZ = {6:1.4,15:3,25:5,50:10,100:20}


def selected_adapter(t):
    if t.execution_adapter != 'auto':
        return 'legacy' if t.execution_adapter == 'legacy' else 'matrix-iq-v2'
    groups = {k: [n for n in t.nodes if n.kind == k] for k in ('epc','enb','ue')}
    cells = [c for n in groups['enb'] for c in n.cells]
    if (any(n.core_adapter != 'srsepc' for n in groups['epc']) or len(groups['epc']) > 1 or any(u.path_attenuation_db or u.radio_environment for u in groups['ue']) or len(groups['ue']) > 1 or len(cells) > 3 or
        len({c.config.dl_earfcn for c in cells}) > 1 or
        len(cells) > 1 and any(n.config.n_prb != 50 for n in groups['enb'])):
        return 'matrix-iq-v2'
    return 'legacy'


def fingerprint(t):
    # Canonical component identities let a fresh template project retain the same
    # verified configuration while its database IDs remain independent.
    from .network import core_for
    # Canonical numeric values are independent of Python default types and
    # JSON round trips. Do not re-run graph validation inside a fingerprint.
    aliases={};cells={}
    for kind in ('epc','enb','ue'):
        for i,n in enumerate(n for n in t.nodes if n.kind==kind):
            aliases[n.id]=f'{kind}:{i}'
            for j,c in enumerate(n.cells):cells[c.id]=f'{kind}:{i}/cell:{j}'
    nodes=[]
    for n in t.nodes:
        if n.id not in aliases:continue
        core=core_for(t,n) if n.kind!='epc' else None
        nodes.append({'id':aliases[n.id], 'kind':n.kind, 'config':n.config.model_dump(),
                      'core':aliases.get(core.id) if core else None, 'core_adapter':n.core_adapter,
                      'paths':{cells[k]:v for k,v in n.path_attenuation_db.items()},
                      'cells':[{'config':c.config.model_dump(),'attenuation_db':c.rf_attenuation_db} for c in n.cells]})
    def canonical(value):
        if isinstance(value,float) and value.is_integer():return int(value)
        if isinstance(value,dict):return {k:canonical(v) for k,v in value.items()}
        if isinstance(value,list):return [canonical(v) for v in value]
        return value
    return hashlib.sha256(json.dumps(canonical({'adapter':selected_adapter(t),'nodes':nodes}),sort_keys=True,separators=(',',':')).encode()).hexdigest()


def radio_profiles(t, adapter):
    profiles=[];errors=[]
    for n in (n for n in t.nodes if n.kind=='enb'):
        for cell in n.cells:
            cfg=cell.config;dl=2620+(cfg.dl_earfcn-2750)*.1;ul=dl-120
            bandwidth=CHANNEL_MHZ.get(n.config.n_prb)
            if bandwidth and (dl-bandwidth/2<2620-1e-6 or dl+bandwidth/2>2690+1e-6):
                errors.append(f'{cell.name}: nominal {bandwidth} MHz channel at {dl:.1f} MHz exceeds Band 7 boundaries; select an EARFCN with bandwidth margin.')
            profiles.append({'cell_id':cell.id,'enb_id':n.id,'band':7,'earfcn':cfg.dl_earfcn,
                             'dl_mhz':round(dl,1),'ul_mhz':round(ul,1),'channel_bandwidth_mhz':bandwidth,
                             'n_prb':n.config.n_prb,'native_rate_hz':PRB_RATES.get(n.config.n_prb),
                             'transport_rate_hz':23040000 if adapter=='legacy' else PRB_RATES.get(n.config.n_prb)})
    return profiles,errors


def needs_broker(t):
    return selected_adapter(t)=='matrix-iq-v2' or sum(len(n.cells) for n in t.nodes if n.kind=='enb')>1


def broker_tool():
    import os
    from .radio import BROKER
    source=Path(__file__).resolve().parents[2]/'orchestrator/rf_broker.cpp'
    stamp=BROKER.with_suffix('.sha256')
    digest=hashlib.sha256(source.read_bytes()).hexdigest()
    ready=os.access(BROKER,os.X_OK) and stamp.is_file() and stamp.read_text()==digest
    return {'ready':ready,'source_sha256':digest,'sha256':hashlib.sha256(BROKER.read_bytes()).hexdigest() if ready else None}


def evidence_for(t, software):
    if not EVIDENCE.exists(): return None
    key = fingerprint(t)
    for row in json.loads(EVIDENCE.read_text()).get('profiles', []):
        if row.get('fingerprint') == key and row.get('result') == 'passed' and row.get('native_version') == software.get('version') and row.get('native_binary_sha256') == software.get('native_binary_sha256'):
            # A changed broker/configuration/runner invalidates the verification.
            from .network import implementation_hash
            if row.get('implementation_sha256') == implementation_hash() and (not needs_broker(t) or row.get('broker_binary_sha256') == broker_tool()['sha256'] and broker_tool()['ready']): return row
    return None


def assess(t, *, check_environment=True):
    from .discovery import _legacy_validate, installation
    from .radio import BROKER
    adapter = selected_adapter(t)
    env = installation()
    if adapter == 'legacy':
        report = _legacy_validate(t)
        additional=[]
        if any(n.core_adapter!='srsepc' for n in t.nodes if n.kind=='epc'):
            additional.append('Open5GS is not implemented by the legacy adapter; selecting it must never silently start bundled srsEPC.')
        if any(n.path_attenuation_db or n.radio_environment for n in t.nodes if n.kind=='ue'):
            additional.append('Explicit UE radio environments and per-UE paths require matrix-iq-v2; the legacy adapter cannot apply them.')
        if additional:
            report['adapter_blockers'].extend(additional);report['adapter_supported']=False;report['runnable']=False
    else:
        from .network import diagnose, plan
        issues = diagnose(t)
        blockers = list(issues.get('resource_blockers',[]))
        if not env['tun_available']: blockers.append('/dev/net/tun is unavailable in this execution environment')
        if not env['isolation_available']: blockers.append('Unprivileged namespace probe failed: '+env['isolation_diagnostic'])
        if not env['plugin'] or not env['zeromq_enabled']: blockers.append('Compiled ZeroMQ plugin unavailable')
        for kind, value in env['binaries'].items():
            if not value['executable']: blockers.append(kind+' binary missing or not executable')
        for name, value in env['tools'].items():
            if not value: blockers.append('Required Linux tool unavailable: '+name)
        if not __import__('os').access(BROKER, __import__('os').X_OK): blockers.append('Build the native I/Q broker with roguard-studio/scripts/build-broker.py')
        cores = [CORES[n.core_adapter].diagnostic() for n in t.nodes if n.kind == 'epc']
        for core in cores:
            if not core['execution_implemented']: blockers.append(core['reason']+' Missing: '+', '.join(core['missing_dependencies']))
        report = {'valid': not issues['errors'], 'environment_ready':not blockers,
                  'adapter_supported':not issues['unsupported'], 'runnable':not issues['errors'] and not issues['unsupported'] and not blockers,
                  'errors':issues['errors'], 'adapter_blockers':issues['unsupported'], 'runtime_blockers':blockers,
                  'warnings':issues['warnings'], 'suggested_pci_changes':[],
                  'rf_plan':plan(t) if not issues['errors'] and not issues['unsupported'] else None,
                  'execution':'REAL — isolated channel/UE FC32 matrix', 'multi_cell':'Frequency-separated RF environments; no inter-frequency scanning or handover'}
    if needs_broker(t) and not broker_tool()['ready']:
        report['runtime_blockers'].append('Native I/Q broker is missing or stale for this source. Run roguard-studio/scripts/build-broker.py before native execution.')
        report['environment_ready']=False;report['runnable']=False
    profiles,frequency_errors=radio_profiles(t,adapter)
    if frequency_errors:
        report['errors'].extend(frequency_errors);report['valid']=False;report['runnable']=False
    if not check_environment:
        report = {**report, 'environment_ready':True, 'runtime_blockers':[],
                  'runnable':report['valid'] and report['adapter_supported']}
    evidence = evidence_for(t, env)
    status = ('Invalid configuration' if not report['valid'] else
              'Unsupported by the selected adapter' if not report['adapter_supported'] else
              'Blocked by missing prerequisites' if not report['environment_ready'] else
              'Supported and native-verified' if evidence else 'Supported by code but not yet native-verified')
    return {**report, 'compatibility':{'registry_version':REGISTRY_VERSION, 'adapter':adapter,
             'status':status, 'native_verified':bool(evidence), 'profile_fingerprint':fingerprint(t),
             'software_version':env.get('version'), 'evidence':evidence,
             'failure_policy':'fail the whole experiment; record failed component independently; mark other components stopped',
             'band':7, 'sample_rates_hz':PRB_RATES,'radio_profiles':profiles}}


def admit(t):
    report = assess(t, check_environment=False)
    if not report['valid'] or not report['adapter_supported']:
        raise ValueError('; '.join(report['errors'] + report['adapter_blockers']))
    return report


def registry():
    return {'version':REGISTRY_VERSION, 'adapters':[
        {'id':'legacy', 'epc':1, 'ue':1, 'enb':3, 'cells':3, 'broker_prbs':[50], 'band':7,
         'transport_rate_hz':23040000, 'mobility':'planned only', 'handover':False},
        {'id':'matrix-iq-v2', 'epc':2, 'ue':2, 'enb':4, 'cells':4, 'cells_per_enb':3,
         'prbs':list(PRB_RATES), 'rates_hz':PRB_RATES, 'band':7,
         'frequency_model':'independent logical RF environments; fixed-frequency receivers; no cross-carrier summation',
         'maxima':'code admission ceilings; consult exact native evidence for validated profiles',
         'failure_policy':'fail entire experiment; preserve each component failure identity', 'handover':False}],
        'core_adapters':[c.diagnostic() for c in CORES.values()],
        'verification':'exact topology fingerprint + native version + implementation SHA-256',
        'native_profiles': json.loads(EVIDENCE.read_text()).get('profiles',[]) if EVIDENCE.exists() else []}
