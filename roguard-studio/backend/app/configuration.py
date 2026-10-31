"""Generate run-local configs using option names checked against this source tree."""
import configparser
import hashlib
import re
import secrets
from pathlib import Path
from .models import Topology

def render_rr(repo: Path, cells):
    """Generate a cell list from owned cells; never mutate upstream templates."""
    rr = (repo/'srsenb/rr.conf.example').read_text()
    # The audited template contains exactly one top-level cell_list and nr_cell_list.
    start = re.search(r'(?m)^cell_list\s*=', rr)
    end = re.search(r'(?m)^nr_cell_list\s*=', rr)
    if not start or not end or start.start() >= end.start():
        raise ValueError('Upstream rr.conf layout changed; audit the generator')
    blocks = []
    for cell in cells:
        c = cell.config
        values = c.model_dump(exclude_none=True)
        fields = '\n'.join(f'    {k} = {str(v).lower() if isinstance(v, bool) else v};' for k,v in values.items())
        blocks.append('  {\n' + fields + '\n    scell_list = ();\n  }')
    return rr[:start.start()] + 'cell_list =\n(\n' + ',\n'.join(blocks) + '\n);\n\n' + rr[end.start():]

def ini(source: Path, changes: dict, dest: Path):
    c = configparser.ConfigParser(interpolation=None, strict=False)
    c.read(source)
    for section, values in changes.items():
        if section not in c: c.add_section(section)
        for k,v in values.items(): c[section][k]=str(v).lower() if isinstance(v,bool) else str(v)
    with dest.open('w') as f: c.write(f)
    dest.chmod(0o600)

def generate(repo: Path, directory: Path, topology: Topology):
    from .radio import is_multi,MULTICELL_VERIFIED
    if is_multi(topology) and MULTICELL_VERIFIED:
        return generate_multi(repo,directory,topology)
    # Defense in depth: no alternate caller can silently execute only the first
    # process/cell from a larger design.
    if any(sum(n.kind == kind for n in topology.nodes) != 1 for kind in ('epc','enb','ue')):
        raise ValueError('Native generator requires exactly one EPC, eNodeB and UE')
    if any(len(n.cells) != 1 or n.cells[0].config.ho_active or n.cells[0].config.rf_port != 0
           for n in topology.nodes if n.kind == 'enb'):
        raise ValueError('Multi-cell / handover RF adapter has not passed native verification')
    if any(c.rf_attenuation_db!=0 for n in topology.nodes for c in n.cells):
        raise ValueError('Path attenuation requires the multi-cell I/Q broker; use 0 dB for direct single-cell mode')
    directory.mkdir(mode=0o700, parents=True, exist_ok=False)
    epc = next(n for n in topology.nodes if n.kind=='epc')
    enb = next(n for n in topology.nodes if n.kind in ('enb','suspicious','temporary'))
    ue = next(n for n in topology.nodes if n.kind=='ue')
    e,u,p = enb.config,ue.config,epc.config
    cell = enb.cells[0].config
    # Fresh synthetic credentials are kept only in private run files; never in public topology JSON.
    key,opc=secrets.token_hex(16),secrets.token_hex(16)
    (directory/'user_db.csv').write_text(f'rg_lab,mil,{u.imsi},{key},opc,{opc},8000,000000001234,7,dynamic\n')
    (directory/'user_db.csv').chmod(0o600)
    for name in ('sib','rb'):
        (directory/f'{name}.conf').write_bytes((repo/f'srsenb/{name}.conf.example').read_bytes())
    (directory/'rr.conf').write_text(render_rr(repo, enb.cells))
    def log(c): return {'all_level':c.log_level,'all_hex_limit':0,'filename':'stdout'}
    def rf(c): return {'device_name':'zmq','device_args':f'fail_on_disconnect=true,tx_port=tcp://127.0.0.1:{c.tx_port},rx_port=tcp://127.0.0.1:{c.rx_port},id={c.device_id},base_srate={c.base_srate}','tx_gain':c.tx_gain,'rx_gain':c.rx_gain}
    ini(repo/'srsepc/epc.conf.example',{
        'mme':{'mcc':p.mcc,'mnc':p.mnc,'tac':p.tac,'mme_bind_addr':p.mme_addr,'apn':p.apn},
        'spgw':{'gtpu_bind_addr':p.mme_addr,'sgi_if_name':'rg_epc','sgi_if_addr':'172.16.0.1'},
        'hss':{'db_file':directory/'user_db.csv'},'log':log(p),
    },directory/'epc.conf')
    ini(repo/'srsenb/enb.conf.example',{
        'enb':{'enb_id':e.enb_id,'mcc':e.mcc,'mnc':e.mnc,'mme_addr':e.mme_addr,'gtp_bind_addr':e.bind_addr,'s1c_bind_addr':e.bind_addr,'n_prb':e.n_prb},
        'enb_files':{f'{n}_config':directory/f'{n}.conf' for n in ('sib','rr','rb')},
        'rf':{**rf(e),'dl_earfcn':cell.dl_earfcn},'log':log(e),
        'expert':{'metrics_csv_enable':e.metrics_enabled,'metrics_csv_filename':directory/'enb_metrics.csv','report_json_enable':e.report_json,'report_json_filename':directory/'enb_report.json'},
        'channel.dl':{'enable':e.awgn_enabled},'channel.dl.awgn':{'enable':e.awgn_enabled,'snr':e.awgn_snr},
    },directory/'enb.conf')
    ini(repo/'srsue/ue.conf.example',{
        'rf':rf(u),'rat.eutra':{'dl_earfcn':u.earfcn},
        'usim':{'mode':'soft','algo':'milenage','imsi':u.imsi,'k':key,'opc':opc},
        'nas':{'apn':u.apn,'apn_protocol':'ipv4','force_imsi_attach':True},
        'gw':{'netns':'rgstudio-ue','ip_devname':'rg_ue'},'log':log(u),
        'general':{'metrics_csv_enable':u.metrics_enabled,'metrics_csv_filename':directory/'ue_metrics.csv','metrics_csv_flush_period_sec':1},
    },directory/'ue.conf')
    return {path.name:hashlib.sha256(path.read_bytes()).hexdigest() for path in directory.iterdir() if path.suffix in ('.conf','.csv')}

def sanitized_config(path: Path):
    if path.name=='user_db.csv': return '# Synthetic subscriber authentication material omitted\n'
    return re.sub(r'(?mi)^(\s*(?:k|opc|op)\s*=).*$',r'\1 [REDACTED]',path.read_text())


def generate_multi(repo: Path, directory: Path, topology: Topology):
    """Per-process native files plus a concrete I/Q routing manifest."""
    import json
    from .radio import restrictions,plan
    errors=restrictions(topology)
    if errors: raise ValueError('; '.join(errors))
    mapping=plan(topology)
    enbs=[n for n in topology.nodes if n.kind=='enb']
    epc=next(n for n in topology.nodes if n.kind=='epc')
    ue=next(n for n in topology.nodes if n.kind=='ue')
    # Reuse the unchanged credential/core/UE generator on a run-local single-cell
    # view, then replace radio configuration with the explicit broker plan.
    baseline=topology.model_copy(deep=True)
    baseline.nodes=[n for n in baseline.nodes if n.kind!='enb' or n.id==enbs[0].id]
    base_enb=next(n for n in baseline.nodes if n.kind=='enb');base_enb.cells=base_enb.cells[:1];base_enb.cells[0].rf_attenuation_db=0
    baseline.links=[]
    generate(repo,directory,baseline)
    def log(c):return {'all_level':c.log_level,'all_hex_limit':0,'filename':'stdout'}
    for n,process in zip(enbs,mapping['processes']):
        role=process['role'];paths=[p for p in mapping['paths'] if p['enb_id']==n.id];e=n.config
        rr_name=f'{role}-rr.conf';(directory/rr_name).write_text(render_rr(repo,n.cells))
        args=['fail_on_disconnect=true',f'id={e.device_id}','base_srate=23040000']
        for path in paths:
            suffix=str(path['rf_port']) if len(paths)>1 else ''
            args += [f'tx_port{suffix}=tcp://127.0.0.1:{path["native_tx"]}',f'rx_port{suffix}=tcp://127.0.0.1:{path["broker_ul"]}']
        ini(repo/'srsenb/enb.conf.example',{
            'enb':{'enb_id':e.enb_id,'mcc':e.mcc,'mnc':e.mnc,'mme_addr':e.mme_addr,'gtp_bind_addr':e.bind_addr,'s1c_bind_addr':e.bind_addr,'n_prb':e.n_prb},
            'enb_files':{'rr_config':directory/rr_name,'sib_config':directory/'sib.conf','rb_config':directory/'rb.conf'},
            'rf':{'device_name':'zmq','device_args':','.join(args),'tx_gain':e.tx_gain,'rx_gain':e.rx_gain},
            'log':log(e),'expert':{'metrics_csv_enable':True,'metrics_csv_filename':directory/process['metrics'],
                                  'report_json_enable':e.report_json,'report_json_filename':directory/f'{role}_report.json'},
            'channel.dl':{'enable':e.awgn_enabled},'channel.dl.awgn':{'enable':e.awgn_enabled,'snr':e.awgn_snr},
        },directory/process['config'])
        # In multi-cell RF mode the process-wide EARFCN override must be absent.
        conf=configparser.ConfigParser();conf.read(directory/process['config']);conf.remove_option('rf','dl_earfcn')
        with (directory/process['config']).open('w') as f:conf.write(f)
    conf=configparser.ConfigParser();conf.read(directory/'ue.conf')
    conf['rf']['device_args']=f'fail_on_disconnect=true,tx_port=tcp://127.0.0.1:{mapping["ue_tx"]},rx_port=tcp://127.0.0.1:{mapping["broker_dl"]},id={ue.config.device_id},base_srate=23040000'
    with (directory/'ue.conf').open('w') as f:conf.write(f)
    (directory/'rf-plan.json').write_text(json.dumps(mapping,indent=2))
    return {p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in directory.iterdir() if p.suffix in ('.conf','.csv','.json')}
