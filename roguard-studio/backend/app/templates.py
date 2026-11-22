"""Versioned designs. Instantiation is additive and always creates fresh IDs."""
from uuid import uuid4
from .models import Topology,Component,Config,Cell,CellConfig,Link

DEFINITIONS = [
    ('single-cell','Single-cell LTE baseline',[1],1,50,[3350],1),
    ('sibling-cells','Two cells in one eNodeB',[2],1,50,[3350],1),
    ('two-enbs','Two eNodeBs / two cells',[1,1],1,50,[3350,3350],1),
    ('mixed-cells','Two eNodeBs / three cells',[2,1],1,50,[3350,3350],1),
    ('three-enbs','Three eNodeBs / three cells',[1,1,1],1,50,[3350]*3,1),
    ('two-ues','Two independent UEs',[1],2,50,[3350],1),
    ('two-ues-two-enbs','Two UEs selecting different cells',[1,1],2,50,[3350]*2,1),
    ('bandwidth-25','25 PRB co-channel experiment',[1,1],1,25,[3350]*2,1),
    ('two-frequencies','Two fixed-frequency RF environments',[1,1],2,50,[3350,3400],1),
    ('four-cells','Four eNodeBs / four cells',[1]*4,1,50,[3350]*4,1),
    ('two-cores','Two independently isolated cores',[1,1],2,50,[3350]*2,2),
]


def build_template(template_id):
    definition=next((d for d in DEFINITIONS if d[0]==template_id),None)
    if definition is None: raise KeyError(template_id)
    ident,name,distribution,ue_count,prb,frequencies,core_count=definition
    cores=[Component(id=f'tpl-epc-{i}',kind='epc',name=f'Core {i+1}',position={'x':40,'y':40+i*420}) for i in range(core_count)]
    enbs=[];index=0
    for i,count in enumerate(distribution):
        cells=[]
        for j in range(count):
            cells.append(Cell(id=f'tpl-cell-{index}',name=f'Cell {index+1}',config=CellConfig(cell_id=j+1,pci=[1,6,11,16][index],rf_port=j,dl_earfcn=frequencies[i]),rf_attenuation_db=0 if index==0 or core_count>1 or len(set(frequencies))>1 else 20));index+=1
        core=cores[i%core_count]
        enbs.append(Component(id=f'tpl-enb-{i}',kind='enb',name=f'eNodeB {i+1}',position={'x':390,'y':40+i*280},core_id=core.id,
                              config=Config(enb_id=411+i,bind_addr=f'127.0.1.{i+1}',device_id=f'enb{i}',n_prb=prb),cells=cells))
    ues=[]
    for i in range(ue_count):
        core=cores[i%core_count];frequency=sorted(set(frequencies))[i%len(set(frequencies))]
        u=Component(id=f'tpl-ue-{i}',kind='ue',name=f'UE {i+1}',position={'x':800,'y':100+i*350},core_id=core.id,
                    config=Config(tx_port=2001,rx_port=2000,device_id=f'ue{i}',imsi=f'00101012345678{i}',earfcn=frequency))
        if ident=='two-ues-two-enbs':u.path_attenuation_db={c.id:(0 if j==i else 20) for j,n in enumerate(enbs) for c in n.cells}
        ues.append(u)
    links=[Link(id=f'tpl-s1-{i}',source=n.core_id,target=n.id,kind='s1') for i,n in enumerate(enbs)]
    links += [Link(id=f'tpl-rf-{i}-{j}',source=n.id,target=u.id,kind='zmq') for i,n in enumerate(enbs) for j,u in enumerate(ues) if n.core_id==u.core_id and any(c.config.dl_earfcn==u.config.earfcn for c in n.cells)]
    return Topology(id='template-'+ident,name=name,nodes=[*cores,*enbs,*ues],links=links,
                    execution_adapter='matrix-iq-v2' if ident not in [d[0] for d in DEFINITIONS[:5]] else 'legacy')


def catalog():
    from .compatibility import assess
    result=[]
    for index,d in enumerate(DEFINITIONS):
        t=build_template(d[0]);report=assess(t)
        # Advanced templates are published only after exact native acceptance.
        if index>=5 and not report['compatibility']['native_verified']:continue
        result.append({'id':d[0],'version':1,'name':d[1],
                       'purpose':'Create a fresh editable, synthetic LTE laboratory with explicit cell/core ownership.',
                       'topology':t.model_dump(),'required_adapter':report['compatibility']['adapter'],
                       'expected_behavior':'Native S1 setup and independent NAS attachment; measured serving PCI/EARFCN; no handover.',
                       'prerequisites':['Linux user/network/mount namespaces','TUN','SCTP','native srsRAN ZeroMQ build','compiled Studio broker for multi-path modes'],
                       'limitations':['Synthetic Band 7 laboratory only','No propagation or handover','Save does not change applied run','Attenuation is static; equal-strength co-channel decoding is not guaranteed','Multiple frequencies use separate fixed-frequency receivers; no scanning or roaming'],
                       'resources':(report['compatibility']['evidence'] or {}).get('resources'),
                       'acceptance':report['compatibility']['evidence'],
                       'label':'Executable — native verified' if report['compatibility']['native_verified'] else 'Code supported — native acceptance pending',
                       'compatibility':report['compatibility']})
    return result


def instantiate(template_id):
    # API only instantiates published entries, never hidden experimental designs.
    if template_id not in {t['id'] for t in catalog()}:raise KeyError(template_id)
    topology=build_template(template_id);ids={n.id:str(uuid4()) for n in topology.nodes}
    ids.update({c.id:str(uuid4()) for n in topology.nodes for c in n.cells})
    for node in topology.nodes:
        node.id=ids[node.id];node.core_id=ids.get(node.core_id)
        node.path_attenuation_db={ids[k]:v for k,v in node.path_attenuation_db.items()}
        for c in node.cells:c.id=ids[c.id]
    for link in topology.links:link.id=str(uuid4());link.source=ids[link.source];link.target=ids[link.target]
    topology.id=str(uuid4());topology.revision=0
    return topology
