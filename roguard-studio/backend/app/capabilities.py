"""Audited option metadata. Parser support and runner verification are distinct."""
CELL_FIELDS = {
    'cell_id': ('Cell identity within the owning eNodeB', 0, 255, 1),
    'pci': ('Physical cell identity; reuse is not a security classification', 0, 503, 1),
    'tac': ('Tracking area; must match the core in the current adapter', 1, 65535, 7),
    'dl_earfcn': ('Band 7 downlink channel in this adapter', 2750, 3449, 3350),
    'ul_earfcn': ('Optional paired uplink; null derives it from downlink', 20750, 21449, None),
    'rf_port': ('Ordered RF channel index within this eNodeB, starting at zero', 0, 4, 0),
    'root_seq_idx': ('PRACH root sequence; overrides shared SIB2 default', 0, 837, 128),
    'ho_active': ('Native handover configuration; execution blocked pending report/completion adapter', None, None, False),
}

def capabilities():
    return {
        'adapter': 'direct-single-cell-or-cochannel-iq-v1',
        'executable_limits': {'epc': 1, 'enb': 3, 'ue': 1, 'cells_per_enb': 3, 'total_cells':3},
        'design_limits': {'cells_per_enb': 5, 'basis': 'source SRSRAN_MAX_CARRIERS; not an execution guarantee'},
        'fields': {k: {'description': d, 'native_field': k, 'file': 'rr.conf',
                       'path': 'cell_list[]', 'owner': 'cell', 'min': lo, 'max': hi,
                       'default': default, 'restart': 'full dependency chain',
                       'support': 'blocked' if k == 'ho_active' else 'native parser / direct and co-channel generators',
                       'dependencies': 'verified neighbor/report and handover completion adapter' if k == 'ho_active' else
                           'Band 7 paired channel: DL + 18000' if k == 'ul_earfcn' else
                           'ordered, unique; two or three total co-channel cells with broker' if k == 'rf_port' else 'owning eNodeB'}
                   for k, (d, lo, hi, default) in CELL_FIELDS.items()},
        'features': [
            {'name': 'Single-cell ZeroMQ', 'class': 'A', 'studio': 'integrated; environment prerequisites required'},
            {'name': 'Two or three owned cells across one to three eNodeBs', 'class': 'A', 'studio': 'native attachment and selection verified with the FC32 broker; 50 PRBs and one channel'},
            {'name': 'Measurement A1–A5 / intra-eNB handover', 'class': 'B', 'studio': 'native support; report and handover integration pending'},
            {'name': 'Co-channel I/Q summation / UL fan-out / static attenuation', 'class': 'A', 'studio': 'native two/three-cell attachment and selection verified; restart required'},
            {'name': 'Mobility and channel scheduling', 'class': 'C', 'studio': 'planned only; no live channel timeline'},
            {'name': 'S1 handover with bundled srsEPC', 'class': 'D', 'studio': 'core does not support S1 handover; a separately integrated capable EPC is required'},
            {'name': 'Hot reconfiguration / pause', 'class': 'D', 'studio': 'unsupported by current process adapter'},
            {'name': 'Canvas positions / paths / coverage', 'class': 'E', 'studio': 'design coordinates; no propagation model'},
        ],
    }
