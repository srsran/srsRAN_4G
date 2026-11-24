"""Bounded native network extension: independent UE streams and RF environments."""
import configparser
import hashlib
import json
import secrets
from pathlib import Path
from .compatibility import PRB_RATES

UE_PORT_BASE = 32000
# Each independent UE owns two distinct TCP endpoints in its run namespace.
UE_ENDPOINT_CAPACITY = (65536 - UE_PORT_BASE) // 2


def implementation_hash():
    studio = Path(__file__).resolve().parents[2]
    # Bind the admission/generation/native/evidence pipeline. Control-page/API
    # presentation and restore endpoints have separate integration/browser tests.
    paths = [studio/'orchestrator/rf_broker.cpp', studio/'orchestrator/runner.py',
             *[studio/'backend/app'/name for name in ('network.py','configuration.py','models.py','engine.py','compatibility.py','radio.py','discovery.py','core_adapters.py','storage.py','integration_contract.py')]]
    h = hashlib.sha256()
    for p in paths: h.update(p.name.encode()); h.update(p.read_bytes())
    return h.hexdigest()


def core_for(t, node):
    cores = [n for n in t.nodes if n.kind == 'epc']
    return next((c for c in cores if c.id == node.core_id), None) if node.core_id else (cores[0] if len(cores) == 1 else None)


def environment_id(core_id, earfcn, prb):
    return f'{core_id}:{earfcn}:{prb}'


def diagnose(t):
    errors=[]; unsupported=[]; warnings=[]; resource_blockers=[]
    groups={kind:[n for n in t.nodes if n.kind==kind] for kind in ('epc','enb','ue')}
    cores,enbs,ues=[groups[k] for k in ('epc','enb','ue')]
    cells=[c for n in enbs for c in n.cells]
    for core in cores:
        if core.core_adapter != 'srsepc':unsupported.append('Open5GS execution is blocked until isolated subscriber provisioning, owned lifecycle and native attachment are integrated; bundled srsEPC remains available.')
    for k,ns in groups.items():
        if not ns:errors.append(f'Add at least one {k.upper()} to complete the network design.')
    for kind,limit in (('epc',2),('enb',4)):
        if len(groups[kind])>limit:unsupported.append(f'The matrix adapter implements at most {limit} native {kind.upper()} processes; configured {len(groups[kind])}. This is a code ceiling, not native capacity evidence.')
    if len(ues)>UE_ENDPOINT_CAPACITY:
        unsupported.append(f'The UE allocation requires {len(ues)*2} endpoints starting at {UE_PORT_BASE}; it exceeds the TCP port range.')
    if len(cells)>4:unsupported.append('The matrix adapter implements at most four total cells; higher capacity is unverified and not admitted.')
    if len({u.config.imsi for u in ues})!=len(ues):errors.append('Each independent UE requires a unique synthetic IMSI; edit the duplicate subscriber identity.')
    for n in enbs+ues:
        core=core_for(t,n)
        if core is None:
            errors.append(f'{n.name}: select one explicit EPC core owner.');continue
        if (n.config.mcc,n.config.mnc)!=(core.config.mcc,core.config.mnc):errors.append(f'{n.name}: PLMN must match its own core.')
        if not n.config.metrics_enabled:unsupported.append(f'{n.name}: matrix execution requires CSV metrics for independently attributed evidence.')
        if n.config.log_level!='info':unsupported.append(f'{n.name}: info logging is required for native readiness evidence.')
        if n.kind=='ue':
            if not n.config.imsi.startswith(n.config.mcc+n.config.mnc):errors.append(f'{n.name}: IMSI must match the synthetic PLMN.')
            if n.config.apn!=core.config.apn:errors.append(f'{n.name}: APN must match its own EPC.')
        else:
            if not n.cells:errors.append(f'{n.name}: add at least one owned cell.')
            if len(n.cells)>3:unsupported.append(f'{n.name}: at most three co-channel sibling PSS indices are supported; distribute additional cells across eNodeBs.')
            if n.config.n_prb not in PRB_RATES:unsupported.append(f'{n.name}: {n.config.n_prb} PRBs has no implemented matrix sample-rate profile.')
            if len({c.config.dl_earfcn for c in n.cells})>1:unsupported.append(f'{n.name}: mixed EARFCNs within one eNodeB are not implemented; use one eNodeB per frequency.')
            if n.config.mme_addr!=core.config.mme_addr or n.config.bind_addr==core.config.mme_addr:errors.append(f'{n.name}: use its EPC MME address and a different S1/GTP bind address.')
            if len({c.config.pci%3 for c in n.cells})!=len(n.cells):errors.append(f'{n.name}: co-channel sibling cells require distinct PCI modulo 3 (PSS indices).')
            for c in n.cells:
                if c.config.tac!=core.config.tac:errors.append(f'{c.name}: TAC must match its own EPC.')
                if c.config.ul_earfcn is not None and c.config.ul_earfcn!=c.config.dl_earfcn+18000:errors.append(f'{c.name}: Band 7 uplink must equal downlink EARFCN + 18000.')
                if c.config.ho_active:unsupported.append('Handover is not implemented in the matrix adapter; external core selection does not verify it.')
    for core in cores:
        owned=[n for n in enbs if core_for(t,n)==core]
        if not owned:errors.append(f'{core.name}: no owned eNodeB.')
        if not any(core_for(t,u)==core for u in ues):errors.append(f'{core.name}: no assigned native UE.')
        if core.config.log_level!='info':unsupported.append(f'{core.name}: info logging is required for core readiness.')
        for attr,label in (('enb_id','eNodeB process IDs'),('bind_addr','S1/GTP bindings')):
            if len({getattr(n.config,attr) for n in owned})!=len(owned):errors.append(f'{core.name}: {label} must be unique within this core environment.')
        by_frequency={}
        for n in owned:
            for c in n.cells:by_frequency.setdefault(c.config.dl_earfcn,[]).append((n,c))
        for frequency,entries in by_frequency.items():
            if len({n.config.n_prb for n,c in entries})!=1:unsupported.append(f'EARFCN {frequency}: mixed bandwidth on the same carrier is not implemented.')
            if len({c.config.pci for n,c in entries})!=len(entries):errors.append(f'EARFCN {frequency}: duplicate PCI within a core is ambiguous and not admitted.')
            candidates=[u for u in ues if core_for(t,u)==core and u.config.earfcn==frequency]
            if not candidates:errors.append(f'{core.name} EARFCN {frequency}: add an independent UE fixed to this radio environment; cross-frequency scanning is not implemented.')
            if len(entries)>2 and len({c.rf_attenuation_db for n,c in entries})==1:warnings.append(f'EARFCN {frequency}: equal-strength co-channel signals may prevent decoding despite active sample paths.')
    for u in ues:
        core=core_for(t,u)
        entries=[(n,c) for n in enbs if core_for(t,n)==core for c in n.cells if c.config.dl_earfcn==u.config.earfcn]
        if not entries:errors.append(f'{u.name}: no cell exists at its assigned core/search EARFCN.')
        elif core:
            expected=environment_id(core.id,u.config.earfcn,entries[0][0].config.n_prb)
            if u.radio_environment and u.radio_environment!=expected:errors.append(f'{u.name}: assigned radio environment must be {expected}.')
            allowed={c.id for n,c in entries}
            if any(k not in allowed for k in u.path_attenuation_db):errors.append(f'{u.name}: attenuation references a cell outside its radio environment.')
    # Resource admission is conservative; native acceptance records measured usage.
    import psutil
    estimated=(len(enbs)*256+len(ues)*600+len(cores)*64+len(cells)*32)*1024**2
    if psutil.virtual_memory().available<estimated:resource_blockers.append(f'Insufficient available memory for conservative {estimated//1024**2} MiB startup estimate. This estimate is not a benchmark.')
    import resource
    socket_limit=resource.getrlimit(resource.RLIMIT_NOFILE)[0]
    if socket_limit!=resource.RLIM_INFINITY:
        # Budget listener/connected sockets and ZeroMQ signalling descriptors.
        for core in cores:
            frequencies={c.config.dl_earfcn for n in enbs if core_for(t,n)==core for c in n.cells}
            for frequency in frequencies:
                streams=sum(1 for n in enbs if core_for(t,n)==core for c in n.cells if c.config.dl_earfcn==frequency)+sum(1 for u in ues if core_for(t,u)==core and u.config.earfcn==frequency)
                if streams*4+64>socket_limit:
                    resource_blockers.append(f'{core.name} EARFCN {frequency}: estimated broker descriptor demand {streams*4+64} exceeds the process limit {socket_limit}.')
    warnings.append(f'UE count follows the design, subject to memory, process descriptors and TCP endpoint availability. Estimated startup memory: {estimated//1024**2} MiB. Core/cell code ceilings remain 2 cores and 4 cells. Exact native verification is shown separately; one component failure stops the full experiment.')
    return {'errors':list(dict.fromkeys(errors)), 'unsupported':list(dict.fromkeys(unsupported)), 'warnings':warnings,'resource_blockers':resource_blockers}


def plan(t):
    cores=[]; processes=[]; ues=[]; channels=[]; paths=[]
    for i,n in enumerate(n for n in t.nodes if n.kind=='epc'):
        role='epc' if i==0 else f'epc-{i+1}'
        cores.append({'role':role,'component_id':n.id,'kind':'epc','config':f'{role}.conf','network_namespace':None if i==0 else f'rgstudio-core-{i+1}'})
    for kind,dest in (('enb',processes),('ue',ues)):
        for i,n in enumerate(n for n in t.nodes if n.kind==kind):
            role=kind if i==0 else f'{kind}-{i+1}';core=core_for(t,n)
            owner=next(c for c in cores if c['component_id']==core.id)
            item={'role':role,'kind':kind,'component_id':n.id,'core_id':core.id,'config':f'{role}.conf','metrics':f'{role}_metrics.csv','network_namespace':owner['network_namespace']}
            if kind=='ue':item['tun_namespace']='rgstudio-ue' if i==0 else f'rgstudio-ue-{i+1}'
            dest.append(item)
    for n in (n for n in t.nodes if n.kind=='enb'):
        proc=next(p for p in processes if p['component_id']==n.id)
        for c in n.cells:
            j=len(paths);env=environment_id(proc['core_id'],c.config.dl_earfcn,n.config.n_prb)
            path={'cell_id':c.id,'enb_id':n.id,'core_id':proc['core_id'],'environment_id':env,'earfcn':c.config.dl_earfcn,'role':proc['role'],'rf_port':c.config.rf_port,'native_tx':31000+j*2,'broker_ul':31001+j*2,'attenuation_db':c.rf_attenuation_db}
            paths.append(path)
            channel=next((x for x in channels if x['id']==env),None)
            if channel is None:
                i=len(channels);rate=PRB_RATES[n.config.n_prb]
                channel={'id':env,'core_id':proc['core_id'],'earfcn':c.config.dl_earfcn,'band':7,'n_prb':n.config.n_prb,'sample_rate':rate,'block_samples':rate//1000,'role':'broker' if i==0 else f'broker-{i+1}','network_namespace':proc['network_namespace'],'paths':[],'ues':[]};channels.append(channel)
            channel['paths'].append(path)
    for i,u in enumerate(n for n in t.nodes if n.kind=='ue'):
        proc=next(p for p in ues if p['component_id']==u.id)
        channel=next(c for c in channels if c['core_id']==proc['core_id'] and c['earfcn']==u.config.earfcn)
        proc.update(environment_id=channel['id'],sample_rate=channel['sample_rate'],ue_tx=UE_PORT_BASE+1+i*2,broker_dl=UE_PORT_BASE+i*2)
        channel['ues'].append({**proc,'attenuation_db':[u.path_attenuation_db.get(p['cell_id'],p['attenuation_db']) for p in channel['paths']]})
    return {'adapter':'matrix-iq-v2','sample_format':'fc32','sample_rate':channels[0]['sample_rate'] if channels else None,
            'paths':paths,'processes':processes,'ues':ues,'cores':cores,'channels':channels,
            'allocation':'fixed distinct loopback endpoints within run-owned, per-core network namespaces',
            'model':'Independent fixed-frequency environments. DL[u]=sum(g[u,c]*cell[c]); UL[c]=sum(g[u,c]*UE[u]); no inter-frequency mixing, scanning, motion or handover.',
            'failure_policy':'fail entire experiment, preserving individual component identities'}


def generate(repo, directory, topology):
    from .configuration import ini, render_rr
    from .compatibility import admit
    admit(topology)
    issues=diagnose(topology)
    if issues['errors'] or issues['unsupported']:raise ValueError('; '.join(issues['errors']+issues['unsupported']))
    mapping=plan(topology);directory.mkdir(mode=0o700,parents=True,exist_ok=False)
    by_id={n.id:n for n in topology.nodes}
    def log(c):return {'all_level':c.log_level,'all_hex_limit':0,'filename':'stdout'}
    def rf(c,args):return {'device_name':'zmq','device_args':args,'tx_gain':c.tx_gain,'rx_gain':c.rx_gain}
    subscribers={}
    for p in mapping['ues']:
        u=by_id[p['component_id']];subscribers[u.id]=(secrets.token_hex(16),secrets.token_hex(16))
    for p in mapping['cores']:
        core=by_id[p['component_id']];c=core.config;name=p['role']+'-user_db.csv'
        lines=[]
        for u in (n for n in topology.nodes if n.kind=='ue' and core_for(topology,n)==core):
            key,opc=subscribers[u.id];lines.append(f'rg_lab,mil,{u.config.imsi},{key},opc,{opc},8000,000000001234,7,dynamic\n')
        (directory/name).write_text(''.join(lines));(directory/name).chmod(0o600)
        ini(repo/'srsepc/epc.conf.example',{'mme':{'mcc':c.mcc,'mnc':c.mnc,'tac':c.tac,'mme_bind_addr':c.mme_addr,'apn':c.apn},'spgw':{'gtpu_bind_addr':c.mme_addr,'sgi_if_name':'rg_epc','sgi_if_addr':'172.16.0.1'},'hss':{'db_file':directory/name},'log':log(c)},directory/p['config'])
    for name in ('sib','rb'):(directory/f'{name}.conf').write_bytes((repo/f'srsenb/{name}.conf.example').read_bytes())
    for p in mapping['processes']:
        n=by_id[p['component_id']];c=n.config;rr=p['role']+'-rr.conf';(directory/rr).write_text(render_rr(repo,n.cells))
        paths=[x for x in mapping['paths'] if x['enb_id']==n.id];rate=next(x['sample_rate'] for x in mapping['channels'] if x['id']==paths[0]['environment_id'])
        args=['fail_on_disconnect=true',f'id={c.device_id}',f'base_srate={rate}']
        for path in paths:
            suffix=str(path['rf_port']) if len(paths)>1 else ''
            args.extend([f'tx_port{suffix}=tcp://127.0.0.1:{path["native_tx"]}',f'rx_port{suffix}=tcp://127.0.0.1:{path["broker_ul"]}'])
        ini(repo/'srsenb/enb.conf.example',{'enb':{'enb_id':c.enb_id,'mcc':c.mcc,'mnc':c.mnc,'mme_addr':c.mme_addr,'gtp_bind_addr':c.bind_addr,'s1c_bind_addr':c.bind_addr,'n_prb':c.n_prb},'enb_files':{'rr_config':directory/rr,'sib_config':directory/'sib.conf','rb_config':directory/'rb.conf'},'rf':rf(c,','.join(args)),'log':log(c),'expert':{'metrics_csv_enable':True,'metrics_csv_filename':directory/p['metrics'],'report_json_enable':False},'channel.dl':{'enable':c.awgn_enabled},'channel.dl.awgn':{'enable':c.awgn_enabled,'snr':c.awgn_snr}},directory/p['config'])
        conf=configparser.ConfigParser();conf.read(directory/p['config']);conf.remove_option('rf','dl_earfcn')
        with (directory/p['config']).open('w') as f:conf.write(f)
    for p in mapping['ues']:
        u=by_id[p['component_id']];c=u.config;key,opc=subscribers[u.id]
        args=f'fail_on_disconnect=true,id={c.device_id},base_srate={p["sample_rate"]},tx_port=tcp://127.0.0.1:{p["ue_tx"]},rx_port=tcp://127.0.0.1:{p["broker_dl"]}'
        ini(repo/'srsue/ue.conf.example',{'rf':rf(c,args),'rat.eutra':{'dl_earfcn':c.earfcn},'usim':{'mode':'soft','algo':'milenage','imsi':c.imsi,'k':key,'opc':opc},'nas':{'apn':c.apn,'apn_protocol':'ipv4','force_imsi_attach':True},'gw':{'netns':p['tun_namespace'],'ip_devname':'rg_ue'},'log':log(c),'general':{'metrics_csv_enable':True,'metrics_csv_filename':directory/p['metrics'],'metrics_csv_flush_period_sec':1}},directory/p['config'])
    (directory/'rf-plan.json').write_text(json.dumps(mapping,indent=2))
    # Broker CLI input is line-based, bounded and validated in C++; no JSON dependency.
    for channel in mapping['channels']:
        lines=[f'{channel["sample_rate"]} {len(channel["paths"])} {len(channel["ues"])}']
        lines += [f'{p["native_tx"]} {p["broker_ul"]}' for p in channel['paths']]
        lines += [f'{u["ue_tx"]} {u["broker_dl"]} '+ ' '.join(map(str,u['attenuation_db'])) for u in channel['ues']]
        (directory/(channel['role']+'.matrix')).write_text('\n'.join(lines)+'\n')
    return {p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in directory.iterdir() if p.suffix in ('.conf','.csv','.json','.matrix')}
