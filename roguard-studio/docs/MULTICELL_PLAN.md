# Multi-cell RF adapter implementation plan

**Updated status:** Native co-channel I/Q execution and static attenuation are implemented and verified for two or three total cells across one to three eNodeBs. See [runtime documentation and evidence](MULTICELL_RUNTIME.md). The milestones below remain the original design roadmap; handover, external-core integration and scheduled mobility are still pending.

## Milestone 1 — controlled intra-eNB two-cell experiment

Keep srsEPC and the existing runner as the default adapter. Add an opt-in adapter with two cells in one eNodeB, ordered RF ports 0/1, unique cell IDs/PCIs, and one shared downlink channel. Generate indexed native ZeroMQ device arguments. Configure native measurement neighbors using the actual ECI `(enb_id << 8) | cell_id`, DL EARFCN and PCI, and an audited A3 report configuration with `ho_active=true`. Do not confuse measurement objects with executable cells.

Install/use GNU Radio only in a separately prepared test environment, or implement a broker against the audited native REQ/REP protocol. Port the official two-cell broker before adding a third stream. Validate complex float sample encoding, buffering of unequal message sizes, sample counts, base rate and backpressure. No PUB/SUB graph substitution is sufficient.

The downlink must consume both cells continuously and deliver aligned samples `gA*xA + gB*xB` to the UE. Uplink consumes one UE stream and fans aligned samples to both native receiver streams. All cells must keep being drained when their gain is zero. A gain slider must change actual sample amplitudes, not metadata. Record gains, sample counters, rates, resets and dropped samples with run provenance.

A first channel model uses deterministic constant scalar attenuation only: amplitude `g=10^(-attenuation_dB/20)`. State explicitly whether DL and UL are reciprocal. Do not claim distance, fading, calibrated RSRP, channel delay, mobility or measured coverage. Normalization/clipping rules must be documented rather than changing experiment conditions silently.

Acceptance: both cells initialize; UE attaches with one path dominant; swap actual broker gains; verify neighbor measurements, measurement report, handover command and native completion plus attached UE PCI transition. A changed PCI alone is insufficient. Failed broker input must fail the run and terminate only owned descendants.

## Milestone 2 — target one EPC, three eNodeBs, one UE

Create one private run subdirectory/config/log/metrics group per process. Allocate identities and loopback bindings independently, and identify runtime components by topology ID rather than the current fixed `enb` role. One private network namespace per run permits reuse across isolated runs; bind all endpoints inside it. Allocate from a validated per-run endpoint map, reserve all listener ports before launch, and fail on collision without killing an unrelated process.

Example **planned** endpoint map inside that namespace:

| Participant | Native TX (REP listener) | Native RX (REQ client) | S1/GTP binding |
| --- | --- | --- | --- |
| EPC | — | — | 127.0.1.100 |
| eNodeB A, RF 0 | 127.0.0.1:2101 | broker 127.0.0.1:2100 | 127.0.1.1 |
| eNodeB B, RF 0 | 127.0.0.1:2201 | broker 127.0.0.1:2200 | 127.0.1.2 |
| eNodeB C, RF 0 | 127.0.0.1:2301 | broker 127.0.0.1:2300 | 127.0.1.3 |
| UE | 127.0.0.1:2001 | broker 127.0.0.1:2000 | separate owned TUN namespace |

The broker connects REQ receivers to each native TX, sums three DL streams into its REP listener at 2000, receives UE UL from 2001, and supplies one REP stream to each eNodeB RX. Multiple eNodeBs pointing directly at one UE socket do not implement this radio environment.

Verify three S1 registrations and GTP coexistence with the chosen EPC before testing radio selection. Use designed PCI 1/6/11 and EARFCN 3350 as sample values only. Start core → eNodeBs/listeners → broker routing → UE, with bounded readiness deadlines; adjust broker/UE connection order only from protocol tests. Shut down UE → broker/eNodeBs → core. Keep backpressure/failure monitoring active through every stage. Persist each process PID, configuration digest, RF map and native readiness independently.

## Milestone 3 — cross-eNodeB handover and mobility

The bundled **srsEPC cannot perform S1 handover**. Integrate a separately configured, handover-capable EPC before claiming cross-eNodeB connected mobility. Use the official S1 application note as a reference, keeping the user's existing srsEPC baseline available. Merely replacing eNodeB addresses cannot add core handover support.

Integrate neighbor/report configuration and native completion event parsing with clear source provenance. Distinguish cell search, idle reselection, RRC connection release/reattachment, serving-parameter change and successful connected handover. Expose only events actually observed.

Only after the attenuation broker passes native tests should saved mobility paths drive gain schedules. Specify coordinate-to-distance units, path interpolation, propagation law, random seed, sample time/timeline synchronization and validation limits. Keep predicted attenuation separate from UE RSRP/SNR measurements. Never animate a serving-cell transition from a path crossing.

## Release gates

1. Preserve the single-cell native and bridge regression suite.
2. Test I/Q broker sample content/counts, zero gains, stream restarts, timeout, cancellation and listener collisions.
3. Repeat real two-cell attachment and intra-eNB handover with packet/measurement continuity evidence.
4. Repeat three-process shared-core registration and native selection with per-process logs and UE measurements.
5. Verify S1 handover only with a capable integrated EPC and native completion evidence.
6. Version any new strict integration structures; retain the existing authenticated v1 envelope and replay semantics.
7. Expose an adapter as operational only after these checks pass and evidence is saved. Until then the Start API must continue returning a blocking report for these designs.
