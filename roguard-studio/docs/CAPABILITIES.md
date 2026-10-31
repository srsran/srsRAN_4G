> Current multi-cell runtime and detailed evidence: [MULTICELL_RUNTIME.md](MULTICELL_RUNTIME.md). Handover and scheduled mobility remain unintegrated.

# Capability matrix

Audit: [installed source and official references](NATIVE_AUDIT.md). Verification: [workbench acceptance](WORKBENCH_VALIDATION.md). Future adapter: [multi-cell implementation plan](MULTICELL_PLAN.md).

A means native functionality integrated with Studio; B means native functionality with integration pending; C needs an I/Q broker or channel model; D is unsupported by the installed component or current execution interface; E is visualization only. A is not a blanket claim that every exposed parameter combination has been tested.

| Capability | Class | Installed engine | Studio and evidence |
| --- | --- | --- | --- |
| One EPC, one eNodeB, one owned cell, one UE over ZeroMQ | A | Present | Real NAS attachment, S1 readiness, CSV observation, restart and stop verified on host |
| Owned cell ID, PCI, TAC, DL/paired UL EARFCN, root sequence | A | Actual `rr.conf` parser fields | Typed editor, ranges/defaults/dependencies, native preview and restart generation; live PCI 1→2 verified |
| RF port | A/B | Ordered per-process RF channel mapping | Port 0 executable; additional ports design only |
| eNodeB bandwidth, gains, RF transport, AWGN | A | Native process-level configuration | Retained generator and restart controls; not positioned as cell-owned settings or geographic measurements |
| Multiple cells in one eNodeB | A | Native cell lists and per-carrier RF mapping | Two and three cells verified with indexed RF channels and the native I/Q broker |
| Multiple eNodeB processes / shared EPC | A | Separate process configurations and S1/GTP bindings | Two and three eNodeBs verified; individual PIDs, logs, CSV provenance and real UE selection |
| Neighbor measurement entries and A1–A5 reporting | B | Native parser support | Dependencies explained; report editing/ingestion not integrated; neighbors never become process nodes |
| UE search/selection/idle reselection | A/B | Native UE procedures | Single-cell attachment and current reported PCI/channel verified; multi-cell behavior unverified |
| Intra-eNB connected handover | B/C | Native eNodeB/UE machinery | Requires multi-channel broker/report adapter and completion evidence; blocked |
| S1 handover using bundled srsEPC | D | Lightweight core does not support it | A separate capable-core adapter is required; not solved by adding graph edges |
| S1 handover using a capable third-party EPC | B/C | Native eNodeB support; official external-core example | No external core installed or integrated by this change |
| Carrier aggregation | B | Native two-carrier example exists | Not integrated; not a substitute for co-channel multi-cell selection |
| I/Q summation, UL fan-out, per-path attenuation | A | Studio C++/libzmq FC32 broker | Actual sample routing and static reciprocal attenuation verified; no GNU Radio dependency |
| 2D positions and mobility plans | E | No link between coordinates and radio | Saved UE path endpoints/speed/timeline; explicitly planned only; no fabricated RSRP or handover |
| Coverage heatmaps / calibrated distance propagation | C/E | No integrated adapter/calibration | Not generated |
| Hot radio edits / pause/resume | D | No supported Studio execution interface | Full dependency-chain restart required |
| Designed / applied / observed separation | A | Native run snapshot and metrics | Saving and preview never rewrite live run files; unresolved serving identities stay unknown |
| Roguard authenticated snapshot/replay/SSE/schema | A | Preserved source interface | Real native measurements replayed into the existing independent bridge; source IDs, event IDs, sequence and provenance retained |

## Current adapter limits

Direct mode: exactly one EPC, eNodeB, owned cell and UE. Broker mode: one EPC and UE, two or three total cells across one to three eNodeBs; 50 PRBs and one shared EARFCN; ZeroMQ only; synthetic MCC `001`; Band 7 DL 2750–3449 with paired UL `DL+18000` if specified; 23.04 Msps base rate. Supported PRB selections remain 6, 15, 25, 50, 100; 75 fails fixed-rate validation. Only the baseline and specific documented parameter changes have native verification; exposing an upstream parser field does not certify every combination.

A Cell cannot exist without an owning eNodeB. A configured UE transport endpoint is not an attachment or serving-cell assignment. Only native attached UE rows support the observed association, and a unique match is labeled as configuration correlation rather than an ECI measurement.

New process creation tools contain EPC, eNodeB and UE only. Security-labeled legacy eNodeBs read as neutral eNodeBs, without rewriting original stored records. Deprecated flat radio identity fields remain for compatibility; `cells[].config` is authoritative. Old annotations can still be read. Scenario APIs and experiment ground truth remain separately marked synthetic metadata for compatibility; they cannot supply native measurements or automatic threat classifications.
