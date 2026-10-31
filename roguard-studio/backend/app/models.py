"""Public API models. Only synthetic PLMNs and loopback virtual RF are executable."""
from typing import Literal
from uuid import uuid4
from pydantic import BaseModel, ConfigDict, Field, model_validator

class Strict(BaseModel):
    model_config = ConfigDict(extra='forbid', allow_inf_nan=False)

class Position(Strict):
    x: float = Field(0, ge=-100000, le=100000)
    y: float = Field(0, ge=-100000, le=100000)

class Config(Strict):
    mcc: str = Field('001', pattern=r'^001$')
    mnc: str = Field('01', pattern=r'^\d{2,3}$')
    enb_id: int = Field(411, ge=0, le=1048575)
    cell_id: int = Field(1, ge=0, le=255)
    tac: int = Field(7, ge=1, le=65535)
    pci: int = Field(1, ge=0, le=503)
    earfcn: int = Field(3350, ge=2750, le=3449)
    n_prb: Literal[6,15,25,50,75,100] = 50
    tx_gain: float = Field(80, ge=0, le=100)
    rx_gain: float = Field(40, ge=0, le=100)
    base_srate: Literal[23040000] = 23040000
    rf_driver: Literal['zmq'] = 'zmq'
    tx_port: int = Field(2000, ge=1024, le=65535)
    rx_port: int = Field(2001, ge=1024, le=65535)
    device_id: str = Field('enb', pattern=r'^[a-zA-Z][a-zA-Z0-9_-]{0,24}$')
    mme_addr: str = Field('127.0.1.100', pattern=r'^127\.\d{1,3}\.\d{1,3}\.\d{1,3}$')
    bind_addr: str = Field('127.0.1.1', pattern=r'^127\.\d{1,3}\.\d{1,3}\.\d{1,3}$')
    imsi: str = Field('001010123456780', pattern=r'^001\d{12}$')
    apn: str = Field('srsapn', pattern=r'^[a-z][a-z0-9-]{0,39}$')
    log_level: Literal['info','warning','error'] = 'info'
    metrics_enabled: bool = True
    report_json: bool = False
    namespace: Literal['rgstudio-ue'] = 'rgstudio-ue'
    awgn_enabled: bool = False
    awgn_snr: float = Field(30, ge=-10, le=60)
    @model_validator(mode='after')
    def valid_addresses(self):
        import ipaddress
        for addr in [self.mme_addr, self.bind_addr]:
            if not ipaddress.ip_address(addr).is_loopback:
                raise ValueError('Only loopback addresses are supported')
        if self.tx_port == self.rx_port:
            raise ValueError('TX and RX ports must differ')
        return self

Kind = Literal['epc','enb','ue','namespace','channel','event','sector','zmq']

class CellConfig(Strict):
    cell_id: int = Field(1, ge=0, le=255)
    pci: int = Field(1, ge=0, le=503)
    tac: int = Field(7, ge=1, le=65535)
    dl_earfcn: int = Field(3350, ge=2750, le=3449)
    # The current adapter is deliberately limited to paired Band 7 frequencies.
    ul_earfcn: int | None = Field(None, ge=20750, le=21449)
    rf_port: int = Field(0, ge=0, le=4)
    root_seq_idx: int = Field(128, ge=0, le=837)
    ho_active: bool = False

class Cell(Strict):
    rf_attenuation_db: float = Field(0, ge=0, le=80)
    id: str = Field(default_factory=lambda: str(uuid4()), pattern=r'^[a-zA-Z0-9_-]{1,80}$')
    name: str = Field('Cell 01', min_length=1, max_length=80, pattern=r'^[^\x00-\x1f]+$')
    config: CellConfig = Field(default_factory=CellConfig)

class Component(Strict):
    id: str = Field(default_factory=lambda: str(uuid4()), pattern=r'^[a-zA-Z0-9_-]{1,80}$')
    kind: Kind
    name: str = Field(min_length=1, max_length=80, pattern=r'^[^\x00-\x1f]+$')
    position: Position = Field(default_factory=Position)
    config: Config = Field(default_factory=Config)
    group: str | None = Field(None, pattern=r'^[a-zA-Z0-9_-]{1,80}$')
    cells: list[Cell] = Field(default_factory=list, max_length=5)

    @model_validator(mode='before')
    @classmethod
    def migrate_legacy(cls, value):
        if not isinstance(value, dict): return value
        value = dict(value)
        # Read old projects without rewriting the saved source or run evidence.
        if value.get('kind') in ('suspicious', 'temporary'):
            value['kind'] = 'enb'
        if value.get('kind') == 'enb' and 'cells' not in value:
            cfg = value.get('config', {})
            if isinstance(cfg, BaseModel): cfg = cfg.model_dump()
            cid = value.get('id') or str(uuid4())
            value['id'] = cid
            value['cells'] = [{'id': 'cell-' + cid[:75], 'name': 'Cell 01', 'config': {
                'cell_id': cfg.get('cell_id', 1), 'pci': cfg.get('pci', 1),
                'tac': cfg.get('tac', 7), 'dl_earfcn': cfg.get('earfcn', 3350)}}]
        return value

    @model_validator(mode='after')
    def ownership(self):
        if self.kind != 'enb' and self.cells:
            raise ValueError('Only an eNodeB process can own cells')
        return self

class Link(Strict):
    id: str = Field(default_factory=lambda: str(uuid4()), pattern=r'^[a-zA-Z0-9_-]{1,80}$')
    source: str
    target: str
    kind: Literal['s1','zmq','annotation']

class MobilityPlan(Strict):
    ue_id: str
    start: Position = Field(default_factory=Position)
    end: Position = Field(default_factory=lambda: Position(x=100, y=100))
    speed: float = Field(1, ge=0, le=100)
    duration: float = Field(60, ge=1, le=86400)
    mode: Literal['planned'] = 'planned'

class Topology(Strict):
    schema_version: Literal['srsran.studio.topology.v2'] = 'srsran.studio.topology.v2'
    id: str = Field('default', pattern=r'^[a-zA-Z0-9_-]{1,80}$')
    name: str = Field('Untitled LTE laboratory', min_length=1, max_length=100)
    nodes: list[Component] = Field(default_factory=list, max_length=100)
    links: list[Link] = Field(default_factory=list, max_length=200)
    revision: int = Field(0, ge=0)
    mobility: list[MobilityPlan] = Field(default_factory=list, max_length=20)
    @model_validator(mode='after')
    def graph(self):
        ids = [n.id for n in self.nodes]
        ids += [c.id for n in self.nodes for c in n.cells]
        if len(ids) != len(set(ids)) or len({e.id for e in self.links}) != len(self.links):
            raise ValueError('IDs must be unique')
        by_id = {n.id:n for n in self.nodes}
        if len({m.ue_id for m in self.mobility}) != len(self.mobility):
            raise ValueError('Only one mobility plan per UE')
        for plan in self.mobility:
            if plan.ue_id not in by_id or by_id[plan.ue_id].kind != 'ue':
                raise ValueError('Mobility plans must reference a UE')
        pairs = set()
        for e in self.links:
            if e.source not in by_id or e.target not in by_id or e.source == e.target:
                raise ValueError('Connections must join two existing, different components')
            a,b = by_id[e.source].kind, by_id[e.target].kind
            kinds = {a,b}
            if e.kind == 's1' and not ('epc' in kinds and kinds & {'enb','suspicious','temporary'}):
                raise ValueError('S1 joins EPC and eNodeB')
            if e.kind == 'zmq' and not ('ue' in kinds and kinds & {'enb','suspicious','temporary'}):
                raise ValueError('ZeroMQ joins eNodeB and UE; both RF directions are configured on the nodes')
            pair = (tuple(sorted([e.source,e.target])),e.kind)
            if pair in pairs:
                raise ValueError('Duplicate connection')
            pairs.add(pair)
        for n in self.nodes:
            if len({c.config.cell_id for c in n.cells}) != len(n.cells):
                raise ValueError('Cell IDs must be unique within an eNodeB')
            if [c.config.rf_port for c in n.cells] != list(range(len(n.cells))):
                raise ValueError('eNodeB RF ports must be ordered starting at 0')
            if n.group and (n.group not in by_id or by_id[n.group].kind != 'namespace' or n.id == n.group):
                raise ValueError('Groups must reference a namespace node')
        return self

class ExperimentInput(Strict):
    name: str = Field(min_length=1, max_length=100)
    category: Literal['Normal','Legitimate Change','Suspicious'] = 'Normal'
    topology_id: str = 'default'

class ScenarioInput(Strict):
    scenario_id: Literal['stable','authorized-cell','tac-change','unexpected-plmn','pci-collision','unexpected-earfcn','signal-anomaly','broadcast','neighbor','downgrade','authentication']
    target: str
    topology_id: str = 'default'
    parameters: dict[str, str | float | bool] = Field(default_factory=dict, max_length=20)

class Approval(Strict):
    reason: str = Field(min_length=5, max_length=500)

class Authorization(Strict):
    pci: int = Field(ge=0, le=503)
    earfcn: int = Field(ge=2750, le=3449)
    reason: str = Field(min_length=5, max_length=500)
