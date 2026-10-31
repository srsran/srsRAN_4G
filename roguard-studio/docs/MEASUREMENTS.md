# Measurement provenance

All persisted timestamps are UTC ISO 8601 with millisecond precision. Browser times are displayed in the browser's local timezone. Metric timestamps represent ingestion time; the engine's `time` column preserves relative time. Missing, non-numeric and non-finite measurements are null. No chart series is synthesized.

| Field | Verified source | Units / interpretation |
| --- | --- | --- |
| UE PCI, EARFCN | `srsue/src/metrics_csv.cc`, `pci`, `earfcn` | Only attached rows establish observed serving-cell parameters |
| RSRP | Same, `rsrp` | dBm |
| Downlink SNR | Same, `dl_snr` | dB; not relabeled as a separately verified SINR measurement |
| DL / UL throughput | Same, `dl_brate`, `ul_brate` | bit/s, computed from actual MAC rates and TTIs |
| DL / UL BLER | Same, `dl_bler`, `ul_bler` | block error percent; not packet loss |
| UE attachment | Same, `is_attached`, plus NAS console success | Actual engine state, not configured UE presence |
| UE assigned IPv4 | `srsue/src/stack/upper/nas.cc` NAS attach-success output | Last evidenced IP; hidden as current when detached/stopped |
| eNodeB UE count and throughput | `srsenb/src/metrics_csv.cc` | `nof_ue`, `dl_brate`, `ul_brate`; CSV can buffer until flush or shutdown |
| Host CPU / memory | psutil | Explicitly backend host resources |
| Process PID / state / RSS | psutil descendants of owned supervisor | Host PID and bytes; creation identity is checked before cleanup |
| RF socket connections | `ss -tnp` inside the isolated network namespace | Actual connection addresses and process ownership |
| S1 readiness | `srsenb/src/stack/s1ap/s1ap.cc` | `S1Setup procedure completed successfully` |
| EPC readiness | `srsepc/src/mme/mme.cc`, `srsepc/src/spgw/spgw.cc` | MME and SPGW initialization messages |

The UE metric configuration uses `general.metrics_csv_flush_period_sec = 1`, as verified in `srsue/src/main.cc`. The eNB uses `expert.metrics_csv_enable` and `expert.metrics_csv_filename`. CSV rows are ingested once, retain partial-line cursors and preserve unknown columns for export. The browser selects a time window and metric; unobserved measurements display an unavailable state.

RSRQ, CQI, observed PLMN/TAC, a complete neighbor topology, EPC subscriber registries and a verified packet-loss measure are not supplied by the current CSV adapter. Their absence is explicit. Gain and AWGN fields are configured engine inputs and never displayed as measured signal strength.

Virtual-radio signal levels depend on software sample scaling and configured gains. They are engine measurements, not calibrated over-the-air field-strength readings.

## Serving-cell association

The current UE serving cell is resolved only from attached native UE PCI/EARFCN, matched uniquely against cells in the applied run. The API reports the cell's design ID and parent eNodeB ID with explicit configuration-correlation provenance. Those IDs are not independently decoded ECI evidence. If multiple applied cells share the same observed signature, the association stays null. Detachment/stopping clears the current association while retaining historical observations. Editing/saving design cells cannot change this observed association.

Studio no longer automatically assigns Normal, Suspicious or Legitimate Change to native observations. Legacy comparison/approval APIs retain explicitly supplied analyst actions for compatibility; synthetic scenario ground truth remains distinct from native evidence. No handover is inferred from PCI changes. Mobility plans, canvas positions and broker/channel predictions are not native measurements.

## Multi-cell broker and process provenance

The co-channel adapter has actual broker input/output sample counters, input energy and configured amplitude gains. These are transport/model measurements, separate from native UE CSV values. Each eNodeB has a separate CSV and immutable component ID; the v1 metric role remains `enb` for consumer compatibility. eNodeB CSVs may buffer until native shutdown, so their absence while running is not filled with synthetic rows. Current serving-cell correlation uses the unique applied PCI/EARFCN. Cold-run selection of another cell is not reported as handover. See [native multi-cell evidence](MULTICELL_RUNTIME.md).
