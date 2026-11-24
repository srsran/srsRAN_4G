# Advanced radio audit and implementation ledger

24 November 2026. This is a partial implementation ledger, not an R01–R12 completion claim.

## Source audit

The complete `orchestrator/rf_broker.cpp` implements legacy reciprocal static
attenuation and matrix weighted DL/UL sums. Buffers preserve stream order across
variable frame lengths. Matrix inputs reject nonfinite samples; all frames must
be nonempty FC32 with bounded length. REQ/REP carries no sequence number or radio
timestamp: sequence gaps and absolute source time alignment cannot be measured.
The broker aligns streams by ordinal sample, not synchronized source timestamps.

`network.py` generates independent UE ports, subscribers and fixed-frequency
channel manifests. Each channel owns a separate broker. `configuration.py`
preserves the direct/legacy adapter. `runner.py` owns process shutdown, readiness,
namespace isolation and per-role logs. Unexpected child exits fail the run.
`engine.py` ingests native observations and persists runtime state. `main.py`
provides session-protected APIs and immutable experiment captures through SQLite
JSON records in `storage.py`. No database reset or schema replacement is needed.

The Network Builder exposes static per-cell/per-UE attenuation. Radio Environment
positions and mobility plans are design metadata. The transport view renders
broker JSON separately from native UE measurements. Existing protocol fixtures
exercise variable-frame alignment, weighted sums and independent UE inputs.
Historical native evidence in `network-evidence/native-profiles.json` records
specific static LTE profiles, including two UEs and two carriers. Changing the
broker or critical implementation invalidates their current compatibility hash;
they are historical evidence, not verification of this revision.

## R01–R12 matrix

| ID | Feature | Implemented | Native verified | Evidence | Remaining limitations |
|---|---|---|---|---|---|
| R01 | Dynamic attenuation | No; static gains only | No | Broker source | No runtime gain control or transition ramp |
| R02 | Gain scheduler | No | No | Runner control supports shutdown only | Scheduling, pause/resume, applied acknowledgments absent |
| R03 | Path loss | No | No | Position metadata only | No distance-to-I/Q coupling |
| R04 | Mobility | Partial design metadata | No | Models/environment view | No movement-driven gain; no demonstrated handover |
| R05 | Delay | No | No | Broker has no delay stage | LTE timing constraints untested |
| R06 | Fading | No | No | Broker has static real gains | Seeded time-varying coefficients absent |
| R07 | Interference | Partial actual co-channel sums | Historical static profiles only | FC32 sum tests | Baseline/interference native measurement comparison absent |
| R08 | Independent UE routing | Implemented | Yes, two-UE/two-carrier profiles in this revision | Matrix fixture tests; native profile manifest | Fresh two-UE profile passed; arbitrary configurations unverified |
| R09 | Independent carriers | Implemented | Yes, two-UE/two-carrier profiles in this revision | Network generator tests; native profile manifest | Fixed receiver assignment; no retuning/scanning |
| R10 | Observability | Partial, extended here | Two static native profiles passed | Buffer unit/API and transport regression tests | Live CPU/RSS, processing latency, connection monitor, structured errors pending |
| R11 | Mathematical validation | Partial existing fixtures plus buffer unit | Protocol fixtures only | Matrix/legacy sum and variable-frame tests | Formal value/error report, delay/fading checks pending |
| R12 | Calibration | No | No | No calibration model | No physical reference or calibration provenance |

## Changes in this increment

Matrix brokers emit mutex-protected queue depth, bounded capacity, high-water
marks and producer/consumer wait interval counters. Wait intervals count blocking
wait iterations, not dropped samples, underflows, or overflows. Backpressure
prevents buffer growth; no synthetic loss counter is reported. Applied reciprocal
path gains include UE/cell indices, zero delay and no fading. Sequence gaps are
explicitly null because the protocol cannot observe them.

The runner now forwards JSON from every broker role, correcting loss of
`broker-2` diagnostics. The backend persists timestamped diagnostics by run and
exposes `/api/zeromq/diagnostics?run_id=...&limit=...` under existing session
middleware. `current` describes run lifecycle, not a freshness or healthy verdict.
Experiment snapshots include diagnostic history and retain native broker logs.
The existing matrix transport JSON viewer exposes the additional fields.

Queue order is cell input, cell output, UE input, UE output, each in manifest
order. Stable component IDs are resolved through the captured RF plan. Memory is
bounded per queue at 2,949,120 FC32 samples (about 22.5 MiB of sample payload),
with two queues per endpoint. Diagnostic history is durable and currently has no
retention policy; large runs need a retention design before sustained deployment.

## Validation procedure and current results

Run with the Studio virtual environment:

```
python -m pytest roguard-studio/tests/test_matrix_broker.py roguard-studio/tests/test_rf_broker.py roguard-studio/tests/test_network.py -q
python -m pytest roguard-studio/tests/test_broker_diagnostics.py -q
```

The first suite passed 33 tests on retry outside the sandbox. The initial run had
8 passes and one three-UE timeout; its cause is unresolved, so this is not a
reliability certification. Inside the sandbox libzmq aborted while creating its
first socket. Native namespace probe returned `unshare failed: Operation not
permitted` inside the sandbox; the host probe passed outside the sandbox. Fresh native two-UE and two-frequency
acceptance both passed; see [native routing evidence](radio-evidence/native-routing.json).
Independent serving observations, process ownership, sample counters, persisted
UE metrics and owned shutdown were checked by `tests/real_network.py`.
No native RSRP, SINR, BLER, handover, or calibrated power claim follows from these
fixture results. Advanced model implementation remains outstanding.

The backend regression run passed 84 tests (excluding the two transport fixture
modules, tested separately). Final diagnostics/matrix checks passed 9 tests,
including emitted gain values, queue bounds, history isolation and session
protection. The standalone C++ buffer check covers exact cross-frame order and
oversized input rejection. No frontend code changed.
