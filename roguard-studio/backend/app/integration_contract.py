"""Version 1 machine interface. Explicit projections are the security boundary.

Never serialize a run, a log, a topology config or an experiment directly here.
"""
import math
import re
from typing import Literal

from pydantic import BaseModel, ConfigDict, Field

SCHEMA = 'srsran.studio.v1'
UNITS = {
    'cc': 'index', 'pci': 'identifier', 'earfcn': 'channel', 'rsrp': 'dBm',
    'pl': 'dB', 'cfo': 'Hz', 'pci_neigh': 'identifier', 'rsrp_neigh': 'dBm',
    'cfo_neigh': 'Hz', 'dl_mcs': 'index', 'dl_snr': 'dB', 'dl_turbo': 'iterations',
    'dl_brate': 'bit/s', 'dl_bler': 'percent', 'ul_ta': 'microseconds',
    'distance_km': 'km', 'speed_kmph': 'km/h', 'ul_mcs': 'index', 'ul_buff': 'bytes',
    'ul_brate': 'bit/s', 'ul_bler': 'percent', 'rf_o': 'count', 'rf_u': 'count',
    'rf_l': 'count', 'is_attached': 'boolean', 'nof_ue': 'count',
    'proc_rmem': 'percent', 'proc_rmem_kB': 'kB', 'proc_vmem_kB': 'kB',
    'sys_mem': 'percent', 'sys_load': 'percent', 'system_load': 'percent',
    'thread_count': 'count',
}
RADIO_FIELDS = ('mcc', 'mnc', 'enb_id', 'cell_id', 'tac', 'pci', 'earfcn', 'n_prb')

def radio_parameters(node):
    parameters = {k: node['config'].get(k) for k in RADIO_FIELDS}
    if node.get('cells'):
        c = node['cells'][0]['config']
        parameters.update({k:c[k] for k in ('cell_id','tac','pci')})
        parameters['earfcn'] = c['dl_earfcn']
    return parameters

def configured_cells(nodes):
    # Public radio metadata only; subscriber credentials and paths are excluded.
    return [{'id':c['id'], 'enb_id':n['id'], 'parameters':c['config'],
             'provenance':'configured; not a native observation'}
            for n in nodes for c in n.get('cells', [])]

def compatible_runtime(data):
    """Keep v1's strict Runtime fields; extensions live inside applied_topology.

    Also accepts transient development journal entries without rewriting their
    stored evidence, sequence, timestamps or identities.
    """
    keys = ('configured_cells', 'configuration_identity', 'serving_cell')
    extensions = {k:data[k] for k in keys if k in data}
    if not extensions: return data
    return {**{k:v for k,v in data.items() if k not in keys},
            'applied_topology':{**(data.get('applied_topology') or {}), **extensions}}


class IntegrationEvent(BaseModel):
    model_config = ConfigDict(extra='forbid', allow_inf_nan=False)
    schema_version: Literal['srsran.studio.v1'] = SCHEMA
    source_id: str
    sequence: int = Field(ge=1)
    event_id: str
    timestamp: str
    timestamp_semantics: Literal['ingestion_utc'] = 'ingestion_utc'
    kind: Literal['metric', 'runtime', 'observation', 'native-event', 'synthetic-event', 'analyst-event', 'designed-topology']
    source_type: Literal['native-measurement', 'native-process', 'synthetic-experiment', 'analyst-decision', 'configuration']
    run_id: str | None
    topology_id: str | None
    component_id: str | None = None
    data: dict


def observation(value):
    if not value:
        return None
    return {
        'parameters': {k: value['parameters'].get(k) for k in ('pci', 'earfcn')},
        'timestamp': value.get('timestamp'), 'source': value.get('source'),
        'run_id': value.get('run_id'),
    }


def project(kind, value, topology_id=None):
    """Return a safe payload or None for private/irrelevant source records."""
    run_id = value.get('run_id')
    timestamp = value.get('timestamp') or value.get('started_at')
    tid = value.get('topology_id') or topology_id
    if kind == 'metric':
        component = value['component']
        units = {**UNITS, 'time': 'ms' if component == 'ue' else 's'}
        values = {}
        for key, number in value['values'].items():
            if key not in units and not re.fullmatch(r'cpu_\d+', key):
                continue
            units.setdefault(key, 'percent')
            values[key] = number if isinstance(number, (int, float)) and math.isfinite(number) else None
        data = {
            'measurement_id': value['id'], 'component': component,
            'provenance': f'srs{component} metrics CSV', 'values': values,
            'units': {k: units[k] for k in values},
            'engine_time': values.get('time'), 'engine_time_unit': units['time'],
        }
        typ, source = 'metric', 'native-measurement'
    elif kind == 'run':
        from .engine import Engine
        serving = Engine.serving_cell(value)
        timestamp = None  # Lifecycle updates are ingested now; started_at stays separate.
        run_id = value['id']
        tid = value['topology']['id']
        data = {
            'state': value['state'], 'attached': bool(value.get('attached', False)) and value['state'] in ('starting', 'running'),
            'configured_cells': configured_cells(value['topology']['nodes']),
            'configuration_identity': {'topology_id':tid, 'revision':value['topology'].get('revision',0),
                'radio_file_sha256':{k:v for k,v in value.get('configuration_hashes',{}).items()
                                     if k in ('enb.conf','rr.conf','sib.conf','rb.conf','epc.conf','rf-plan.json') or re.fullmatch(r'enb(?:-[23])?(?:-rr)?\.conf',k)}},
            'serving_cell': {k:v for k,v in serving.items() if k != 'name'} if serving else None,
            'components': {k: v for k, v in value.get('components', {}).items() if k in ('epc', 'enb', 'enb-2', 'enb-3', 'ue', 'broker')},
            'observed': observation(value.get('observed')), 'started_at': value.get('started_at'),
            'finished_at': value.get('finished_at'),
            'applied_topology': {
                'id': tid, 'revision': value['topology'].get('revision', 0),
                'rf_plan':value.get('rf_plan'),'broker':value.get('broker'),
                'nodes': [{'id': n['id'], 'kind': n['kind'],
                           'configured_parameters': radio_parameters(n)}
                          for n in value['topology']['nodes']],
                'links': [{k: link[k] for k in ('id', 'source', 'target', 'kind')}
                          for link in value['topology']['links']],
            },
            'configured_radio': [{
                'node_id': n['id'], 'kind': n['kind'],
                'parameters': radio_parameters(n),
                'provenance': 'applied configuration; not a radio observation',
            } for n in value['topology']['nodes'] if n['kind'] in ('enb', 'temporary', 'suspicious')],
        }
        data = compatible_runtime(data)
        typ, source = 'runtime', 'native-process'
    elif kind == 'event':
        name = value['type']
        if name == 'topology-observation':
            data = {'observed': observation(value.get('observed'))}
            typ, source = 'observation', 'native-measurement'
        elif name == 'scenario':
            scenario = value.get('scenario', {})
            data = {k: scenario.get(k) for k in ('id', 'scenario_id', 'state', 'ground_truth')}
            data['scenario_id'] = scenario.get('scenario_id') or value.get('scenario_id')
            data['execution_method'] = 'SIMULATED EVENT'
            if not scenario:
                data['state'] = 'stopped'
            tid = tid or scenario.get('topology_id')
            run_id = run_id or scenario.get('run_id')
            typ, source = 'synthetic-event', 'synthetic-experiment'
        elif name in ('lifecycle', 'attachment', 'failure', 'configuration', 'validation'):
            data = {'event_name': name}
            if name == 'attachment':
                data['attached'] = value.get('attached')
            typ, source = 'native-event', 'native-process'
        elif name in ('baseline', 'authorization'):
            data = {'event_name': name, 'scope': 'Studio analyst action; Roguard baseline is independent'}
            typ, source = 'analyst-event', 'analyst-decision'
        else:
            return None
    elif kind == 'topology':
        tid = value['id']
        data = {'revision': value['revision'], 'configured_cells': configured_cells(value['nodes']), 'nodes': [{
            'id': n['id'], 'kind': n['kind'],
            'configured_parameters': radio_parameters(n),
        } for n in value['nodes']], 'links': [{k: e[k] for k in ('id', 'source', 'target', 'kind')} for e in value['links']]}
        typ, source = 'designed-topology', 'configuration'
    else:
        return None
    return {'kind': typ, 'source_type': source, 'run_id': run_id, 'topology_id': tid, 'timestamp': timestamp, 'data': data}
