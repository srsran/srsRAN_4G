> Initial source audit retained. A native C++/libzmq broker has since been implemented and verified; see [current multi-cell runtime](MULTICELL_RUNTIME.md). GNU Radio is not required by that adapter.

# Native audit — 31 October 2026

The existing executable reports **25.10.0**, source HEAD `bef8680d5f9714f3e040e6f9cbc88d7888439b6d`. No C++ source, installed binary, RF plugin, example configuration or CMake setting was modified. The earlier example hashes in `configs/upstream-manifest.json` are retained as the comparison baseline.

## Installed implementation

| Source inspected | Finding | Studio consequence |
| --- | --- | --- |
| `srsenb/src/enb_cfg_parser.cc:1484` | Each `cell_list` entry parses cell ID, TAC, PCI, DL/optional UL EARFCN, RF port, root sequence, and optional handover configuration | Cells are owned configurations, not processes. The cell editor uses these actual names. |
| Same, `1542` | RF ports must be unique and ordered from zero; cell IDs must be unique within a process | Domain validation enforces order and uniqueness. |
| Same, `1332–1466` | `meas_cell_list` describes ECI/EARFCN/PCI measurement objects; report parser handles A1–A5; enabling handover requires report configuration | A neighbor entry is not a cell process. Unintegrated handover is blocked. |
| Same, `1810`, `1888–1984` | Number of native RF carriers is derived from cell lists; single-cell RF defaults and per-cell PHY configuration are treated differently | Multiple cells need a different RF adapter; repeating the current single-channel config is insufficient. |
| `lib/include/srsran/phy/common/phy_common.h:56` | `SRSRAN_MAX_CARRIERS=5`, four antenna ports, channel buffers sized accordingly | Five owned cells is a **design bound**, not a verified operational capacity. Current executable adapter remains one cell. |
| `lib/src/phy/rf/rf_zmq_imp.c:308` | Indexed `tx_portN`/`rx_portN` are parsed per RF channel | Intra-eNB multi-cell requires distinct indexed RF endpoints. |
| `lib/src/phy/rf/rf_zmq_imp_rx.c`, `rf_zmq_imp_tx.c` | Default receive path requests samples; transmit path replies with binary baseband buffers | A generic PUB/SUB relay is incompatible. Broker buffering and REQ/REP timing must be tested. |
| `srsue/src/stack/rrc/rrc.cc:532`, `550`, `rrc_procedures.cc` | Native search, cell-selection criteria, idle reselection and mobility procedures exist | Native support is not proof that Studio has made multiple cells visible to the UE. |
| `srsenb/src/stack/rrc/rrc_mobility.cc`, `stack/s1ap/s1ap.cc` | eNodeB intra-eNB/S1 mobility machinery and completion events exist | UE identity changes alone cannot establish successful handover. |
| `srsenb/sib.conf.example` | SIB1 minimum receive level; SIB2 shared PRACH root default 128; SIB3 reselection settings | Shared SIB configuration remains in the eNodeB's copied upstream file. Per-cell root sequence overrides its SIB2 default. No unrestricted SIB editor is exposed. |
| `srsue/src/metrics_csv.cc` | Attached UE rows contain PCI/EARFCN and actual signal/rate metrics | Serving-cell association is a unique match to the **applied** cell configuration, with explicit provenance; ECI is not independently measured. |

No `.grc` flowgraph exists in this checkout. GNU Radio is absent from the host Python installation. The sandbox hides TUN and denies `unshare`; the host provides both. Native checks were therefore run on the host using the unchanged private-namespace runner, without root or system changes.

## Official references reviewed

- [ZeroMQ virtual radio and GNU Radio Companion broker](https://docs.srsran.com/projects/4g/en/latest/app_notes/source/zeromq/source/index.html): samples travel over real TCP I/Q connections; namespace isolation separates UE/EPC network interfaces.
- [Intra-eNB and S1 handover](https://docs.srsran.com/projects/4g/en/latest/app_notes/source/handover/source/index.html): intra-eNB uses multiple cells/RF channel pairs and a gain-controlled broker. **srsEPC does not support S1 handover**; the S1 example uses an external capable EPC such as Open5GS. This is a separate blocker from RF mixing.
- [Carrier aggregation](https://docs.srsran.com/projects/4g/en/latest/app_notes/source/2ca/source/index.html): two-carrier aggregation uses secondary-cell configuration, multiple RF channels and UE capabilities. CA is not equivalent to independent co-channel serving-cell selection or handover.

The official documentation is release 23.11; the local source/binary is 25.10.0. Local parser and driver inspection take precedence for generated options. Documentation examples are architectural references, not claimed local test results.
