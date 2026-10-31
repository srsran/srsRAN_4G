"""Allocated, run-local co-channel RF plan. No physical radio or arbitrary endpoints."""
from pathlib import Path

# Enabled only after the checked-in native acceptance has passed.
MULTICELL_VERIFIED = True
BROKER = Path(__file__).resolve().parents[2]/'var/tools/rf_broker'

def is_multi(t):
    return sum(len(n.cells) for n in t.nodes if n.kind=='enb') > 1 or sum(n.kind=='enb' for n in t.nodes)>1

def pci_diagnostics(t):
    """Suggest minimal design-only PCI edits, preserving the first valid cell.

    Later nonconflicting identities are reserved so a suggestion does not create
    another conflict. PSS diversity is scoped to co-channel siblings; global
    signature uniqueness is a constraint of this broker's observation profile.
    """
    entries=[(n,c) for n in t.nodes if n.kind=='enb' for c in n.cells]
    changes=[];errors=[];proposed={};seen=[]
    def effective(n,c):return proposed.get((n.id,c.id),c.config.pci)
    for n,c in entries:
        cfg=c.config
        duplicate=next(((a,b) for a,b in seen if b.config.dl_earfcn==cfg.dl_earfcn and b.config.pci==cfg.pci),None)
        pss=next(((a,b) for a,b in seen if a.id==n.id and b.config.dl_earfcn==cfg.dl_earfcn and b.config.pci%3==cfg.pci%3),None)
        if duplicate or pss:
            other,prior=duplicate or pss
            reserved={effective(a,b) for a,b in entries if (a.id,b.id)!=(n.id,c.id)}
            occupied={effective(a,b)%3 for a,b in entries if a.id==n.id and b.id!=c.id and b.config.dl_earfcn==cfg.dl_earfcn}
            candidate=next((value for value in [1,6,11,*range(504)] if value not in reserved and value%3 not in occupied),None)
            reason='duplicate PCI' if duplicate else 'the same PSS index (PCI modulo 3)'
            text=f'{n.name} / {c.name} (RF {cfg.rf_port}): PCI {cfg.pci} on EARFCN {cfg.dl_earfcn} conflicts with {other.name} / {prior.name} (PCI {prior.config.pci}, PSS {prior.config.pci%3}): {reason}.'
            if candidate is not None:
                proposed[(n.id,c.id)]=candidate
                changes.append({'enb_id':n.id,'cell_id':c.id,'cell_name':c.name,'from_pci':cfg.pci,'to_pci':candidate})
                text+=f' Change {c.name} to PCI {candidate} (PSS {candidate%3}); keep its cell ID and RF port.'
            else:text+=' No unused PSS index remains for these co-channel siblings; the verified profile supports at most three.'
            errors.append(text)
        seen.append((n,c))
    return {'errors':errors,'changes':changes}

def restrictions(t):
    errors=[]
    enbs=[n for n in t.nodes if n.kind=='enb']; cells=[c for n in enbs for c in n.cells]
    if sum(n.kind=='epc' for n in t.nodes)!=1 or sum(n.kind=='ue' for n in t.nodes)!=1:
        errors.append('Broker adapter requires exactly one EPC and one UE.')
    if not 2<=len(cells)<=3 or not 1<=len(enbs)<=3 or any(not n.cells for n in enbs):
        errors.append('Broker adapter supports two or three total cells across one to three eNodeBs.')
    if any(n.config.n_prb!=50 for n in enbs): errors.append('Verified broker adapter requires 50 PRBs on every eNodeB.')
    if len({c.config.dl_earfcn for c in cells})!=1: errors.append('Broker adapter requires all cells on the same DL EARFCN.')
    errors.extend(pci_diagnostics(t)['errors'])
    if len({n.config.enb_id for n in enbs})!=len(enbs): errors.append('eNodeB process IDs must be unique.')
    if len({n.config.bind_addr for n in enbs})!=len(enbs): errors.append('eNodeB S1/GTP loopback bindings must be unique.')
    if any(c.config.ho_active for c in cells): errors.append('Handover is not enabled in the broker selection adapter; srsEPC has no S1 handover support.')
    epc=next((n for n in t.nodes if n.kind=='epc'),None);ue=next((n for n in t.nodes if n.kind=='ue'),None)
    if epc and ue:
        if len({(n.config.mcc,n.config.mnc) for n in [epc,ue,*enbs]})!=1: errors.append('EPC, eNodeBs and UE must share the synthetic PLMN.')
        if not ue.config.imsi.startswith(ue.config.mcc+ue.config.mnc):errors.append('UE IMSI must match the configured PLMN.')
        if epc.config.apn!=ue.config.apn: errors.append('EPC and UE APN must match.')
        if any(n.config.mme_addr!=epc.config.mme_addr or n.config.bind_addr==epc.config.mme_addr for n in enbs):errors.append('Every eNodeB must use the EPC MME address and a distinct GTP binding.')
        if any(c.config.tac!=epc.config.tac for c in cells): errors.append('All cell TACs must match the bundled EPC.')
        if cells and ue.config.earfcn!=cells[0].config.dl_earfcn: errors.append('UE search channel must match the co-channel RF environment.')
    if any(n.config.log_level!='info' for n in [*enbs,*([epc] if epc else []),*([ue] if ue else [])]): errors.append('Info logging is required for native readiness evidence.')
    return errors

def plan(t):
    paths=[]; processes=[]
    for i,n in enumerate(n for n in t.nodes if n.kind=='enb'):
        role='enb' if i==0 else f'enb-{i+1}'
        processes.append({'role':role,'component_id':n.id,'config':f'{role}.conf','metrics':f'{role}_metrics.csv'})
        for cell in n.cells:
            j=len(paths)
            paths.append({'cell_id':cell.id,'enb_id':n.id,'role':role,'rf_port':cell.config.rf_port,
                          'native_tx':31000+j*2,'broker_ul':31001+j*2,
                          'attenuation_db':cell.rf_attenuation_db})
    return {'adapter':'cochannel-iq-v1','sample_rate':23040000,'sample_format':'fc32',
            'ue_tx':32001,'broker_dl':32000,'paths':paths,'processes':processes,
            'allocation':'fixed collision-free ports inside the run-owned isolated network namespace',
            'model':'DL sum of amplitude-scaled FC32 paths; UL fan-out with the same attenuation; no geographical model'}
