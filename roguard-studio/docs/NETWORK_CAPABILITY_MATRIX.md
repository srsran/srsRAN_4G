# Network capability matrix

Date: 22 November 2026. All rows in the native table are actual tests from this implementation; see [public native manifest](network-evidence/native-profiles.json). Source base commit: `16f2eb9fa25ce3a0f20a440e961459a4749d3434`; uncommitted implementation identity is recorded by SHA-256, along with srsRAN 25.10.0 executable hashes and the compiled broker hash.

## Exact native profiles

| Template / test | EPCs | eNodeBs / owned cell distribution | UEs | EARFCNs | PRBs | Transport Msps | Native outcome | Run ID |
|---|---:|---|---:|---|---:|---|---|---|
| single-cell | 1 | 1 / 1 | 1 | 3350 | 50 | 23.04 | S1 + all UE NAS/serving evidence passed | `d08b5700-588d-4844-b983-1d073153504f` |
| sibling-cells | 1 | 1 / 2 | 1 | 3350 | 50 | 23.04 | S1 + all UE NAS/serving evidence passed | `ac005fbe-a5c0-49d7-8b11-eb9e1d193ef5` |
| two-enbs | 1 | 2 / 1+1 | 1 | 3350 | 50 | 23.04 | S1 + all UE NAS/serving evidence passed | `b5eb01c5-6be6-4ff1-82a9-a759ce69b2fb` |
| mixed-cells | 1 | 2 / 2+1 | 1 | 3350 | 50 | 23.04 | S1 + all UE NAS/serving evidence passed | `78b671b2-9017-409f-97d3-4290dd3b77b5` |
| three-enbs | 1 | 3 / 1+1+1 | 1 | 3350 | 50 | 23.04 | S1 + all UE NAS/serving evidence passed | `0a3c4ef8-f140-4500-963b-fa7c0ba45dc1` |
| two-ues | 1 | 1 / 1 | 2 | 3350 | 50 | 11.52 | S1 + all UE NAS/serving evidence passed | `4b5384e5-aa08-48c8-9bcf-9e5d083a548a` |
| two-ues-two-enbs | 1 | 2 / 1+1 | 2 | 3350 | 50 | 11.52 | S1 + all UE NAS/serving evidence passed | `c70f41ed-6ba4-427d-ab1a-cfe7464d12d2` |
| bandwidth-25 | 1 | 2 / 1+1 | 1 | 3350 | 25 | 5.76 | S1 + all UE NAS/serving evidence passed | `f365fd9e-21c6-429a-82de-c21cfb98ee93` |
| two-frequencies | 1 | 2 / 1+1 | 2 | 3350, 3400 | 50 | 11.52 | S1 + all UE NAS/serving evidence passed | `bb568974-7025-4f80-bfa3-ff2a6755246b` |
| four-cells | 1 | 4 / 1+1+1+1 | 1 | 3350 | 50 | 11.52 | S1 + all UE NAS/serving evidence passed | `71b74821-55ba-4891-818a-317b7dd21844` |
| two-cores | 2 | 2 / 1+1 | 2 | 3350 | 50 | 11.52 | S1 + all UE NAS/serving evidence passed | `8166eba1-6f92-46b6-b12b-103ff52b45b9` |

Names, design positions, revisions and fresh UUIDs do not change a profile. Radio parameters, subscriber identities, core/channel/path associations, native binaries, broker binary and critical implementation changes do. Equivalent integer/float JSON encodings are canonicalized. A shape alone does not qualify as native-verified.

All co-channel multi-cell demonstrations use explicit static attenuation; no blanket equal-gain, neighbor decode or handover guarantee is made. The two-frequency row uses separate fixed-frequency UEs/brokers, not a wideband mixer or scanning receiver. The two-core row is isolated, not roaming.

## Code profiles and boundaries

| Capability | Implemented boundary | Verification boundary |
|---|---|---|
| Legacy direct mode | 1 core/eNodeB/cell/UE, ZeroMQ, original transport | Exact single-cell row verified; other parameter combinations require their own record |
| Legacy co-channel broker | 2–3 total cells, 1–3 eNodeBs, 1 EPC/UE, 50 PRBs | Five required legacy shapes retained and tested |
| Matrix mode | At most 2 EPCs, 4 eNodeBs, 4 cells; UE count admitted by memory, descriptor and TCP endpoint budgets; 3 co-channel siblings per eNodeB | These are code ceilings. Exact accepted profiles are listed above, not every combination under the ceilings |
| Bands/frequencies | Band 7; paired UL = DL + 18000; nominal bandwidth must fit | 3350 and 3400 in listed profiles; no other-band certification |
| Bandwidth/rates | 6/15/25/50/100 PRBs = 1.92/3.84/5.76/11.52/23.04 Msps in matrix | 25 and 50 in matrix verified; 6/15/100 exact combinations await native acceptance |
| Mixed bandwidth | Rejected on the same carrier; separate carrier groups have separate rates | Arbitrary mixed-rate combinations unverified |
| Multiple frequencies in one eNodeB | Unsupported; use separate eNodeBs | No intra-eNodeB mixed-carrier claim |
| Multiple cores | Separate subscriber/config/process/network environments; owned scoped stop | Two independent attach chains and core-2 scoped stop verified |
| Open5GS | Selection/prerequisite/lifecycle policy only; native execution disabled | Missing core/MongoDB and unfinished provisioning/lifecycle; no attachment evidence |
| Mobility/propagation | Positions and paths are planned metadata | No RF-driven movement, propagation, Doppler or coverage model |
| Handover/roaming | Unsupported | No completed native handover or cross-core roaming demonstrated |
| Failure policy | Unexpected component exit fails the experiment; healthy children stop with distinct identities | UE-2 failure and owned core shutdown recorded separately |
| Resources | Conservative available-memory prerequisite; bounded queue/process counts | Short acceptance samples below, not sustained traffic/capacity certification |

## Measured short-run resources

RSS is summed over owned processes; shared pages may be counted more than once. CPU is calculated from user+system time deltas and summed across processes, so 100% means one logical CPU and totals may exceed 100%. Per-process broker data is retained in the manifest. These short experiments do not measure maximum sustainable UE traffic.

| Profile | Peak summed RSS MiB | Peak sampled CPU % | Peak broker RSS MiB | Peak broker CPU % |
|---|---:|---:|---:|---:|
| single-cell | 995.0 | 96.8 | 0.0 | 0.0 |
| sibling-cells | 1177.7 | 176.6 | 14.6 | 65.3 |
| two-enbs | 1451.2 | 207.5 | 14.5 | 70.4 |
| mixed-cells | 1625.7 | 292.3 | 17.8 | 113.1 |
| three-enbs | 1900.6 | 334.1 | 17.2 | 118.2 |
| two-ues | 1456.6 | 188.5 | 12.6 | 62.2 |
| two-ues-two-enbs | 1903.0 | 249.0 | 14.1 | 90.7 |
| bandwidth-25 | 1332.3 | 178.0 | 9.7 | 34.6 |
| two-frequencies | 1912.2 | 306.9 | 21.7 | 104.6 |
| four-cells | 2340.3 | 322.4 | 14.7 | 113.3 |
| two-cores | 1980.9 | 264.7 | 21.4 | 95.5 |

Broker columns sum each broker process's individual sampled peak; those peaks need not occur at the same instant. The original host inventory recorded eight logical CPUs and about 7.4 GB available memory. Resource estimates are labeled estimates and cannot substitute for native capacity tests.

## Feature completion

| ID | Feature | Implemented | Native verified | Tests | Remaining limitation |
|---|---|---|---|---|---|
| N01 | Multiple independent UEs | Yes | Yes, 2 UEs | FC32/subscriber/state/native/browser | Design-sized UE allocation subject to resource admission; fixed channels; fail-whole-experiment policy |
| N02 | Advanced multi-eNodeB topologies | Yes | Yes | Preserved five shapes + extended native cases | Exact profiles, not arbitrary identities/gains |
| N03 | More than three cells | Yes | Yes, 4 across 4 eNodeBs | Bounded buffers, native S1/samples/NAS + resources | 4 total cells; max 3 siblings; no all-cell decode claim |
| N04 | Multiple frequencies | Yes | Yes, 3350/3400 | Channel separation, band margins, native receivers | Band 7, independent fixed receivers; no scanning |
| N05 | Variable bandwidth | Yes | Yes, additional 25-PRB profile | Rate/generator/broker + native PRB-25 decode | Other implemented PRBs await exact native combinations |
| N06 | Multiple EPC environments | Yes | Yes, 2 isolated cores | Ownership/isolation + native attach/scoped stop | No roaming or dynamic/per-core restart |
| N07 | External EPC integration | Partial policy/diagnostics | No — blocked | Negative selection/admission + srsEPC regression | Open5GS/MongoDB absent; provisioning/lifecycle/attach not implemented |
| N08 | Advanced network templates | Yes | Yes, all 11 published executable profiles | Versioning/fresh IDs/round trip + native/browser | Changed profiles/implementation lose certification |
| N09 | Compatibility registry | Yes | Yes, linked exact profiles | Classification/version/hash/prerequisite/agreement tests | Unlisted combinations remain explicitly unverified |

## Execution results

- Prepared baseline: 60 tests passed.
- Final backend/broker suite: 84 tests passed; one Starlette/httpx deprecation warning.
- TypeScript checking passed.
- Native acceptance: all eleven listed profiles passed. UE failure and scoped-core shutdown are separate evidence records.
- Browser: all 15 tests passed; independently recorded Playwright child exit code 0. The earlier all-pass report with outer session exit 143 is retained separately.
- Production build: unchanged retry passed after Next.js compiled and then stopped on an installed-version security advisory. No dependencies/security protections were changed.
- Actual Roguard: native UE component IDs persisted for all 18 runs across the two final browser executions; source journal/checkpoint caught up, zero rejected events. See [test evidence](network-evidence/README.md) for exact records, preserved failures and cleanup.

## Gate disposition

G0 baseline, G1 compatibility, G2 templates, G3 independent UEs, G4 eNodeB extension, G5 bandwidth, G6 fixed-frequency environments, G8 four-cell capacity and G9 isolated cores have implementation and native evidence within the boundaries above. G7 external-core execution is blocked; it did not prevent independent extensions. G10 regression evidence is recorded in the final evidence ledger.
