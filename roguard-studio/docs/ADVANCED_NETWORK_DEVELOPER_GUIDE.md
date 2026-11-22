# Advanced network developer guide

## Start without replacing your workspace

Run from the repository root:

```bash
./roguard-studio/scripts/start.sh
```

Use the printed frontend/API ports. Restart the Studio launcher to load updated Python/Next code; do not terminate unrelated services. Existing `var/studio.sqlite3`, saved projects, experiments and old run directories remain valid. New fields have read-time defaults; there is no database reset or destructive migration.

The existing native `srsepc`, `srsenb`, `srsue` and ZeroMQ plugin are reused. Only the Studio broker needs recompilation:

```bash
roguard-studio/.venv/bin/python roguard-studio/scripts/build-broker.py
```

The launcher also builds the broker when its source hash changes. It needs g++/C++17 and libzmq development headers. Native acceptance requires Linux, `/dev/net/tun`, SCTP, permitted unprivileged user/network/mount namespaces, `ip`, `mount`, `stdbuf`, and enough available memory. Run as your ordinary laboratory user, not sudo. A sandbox with no TUN/namespaces cannot execute these tests.

## Choose a template

1. Open **Network Templates**.
2. Preview a versioned design, its ownership, purpose, expected behavior, prerequisites and limitations.
3. Select **Create editable project**. It creates fresh project/process/cell/link IDs without changing existing projects or an applied run.
4. Open **Simulation Runtime** and read **Configuration compatibility**.
5. Save/validate. Start through the existing Roguard connection dialog, entering the actual dashboard and Studio API URLs.
6. Wait for native readiness and **Independent UE evidence**, rather than inferring attachment from icons.
7. Capture/export an experiment, then stop the owned experiment.

Advanced templates are published only after a matching successful native acceptance record exists for the exact profile, native binary hashes/version and implementation hash. A code upgrade can remove the verified label until acceptance is rerun. Saved projects remain editable. Host prerequisites can still block a previously verified profile.

## Demonstration checklist

Use one experiment at a time. For each row, preview the template, create its editable project, read compatibility, start, wait for native readiness, inspect evidence, then capture/export and stop. Capture/restore creates a separate design and preserves the original snapshot.

| Feature | Template / action | Evidence to show |
|---|---|---|
| N01 independent UEs | Two independent UEs; then Two UEs selecting different cells | Two native UE PIDs, separate NAS/IP/serving entries and separately selectable measurement sources |
| N02 more base-station arrangements | Two eNodeBs / three cells; Three eNodeBs / three cells | Per-eNodeB process readiness, owned cells and S1 evidence |
| N03 four cells | Four eNodeBs / four cells | Four eNodeB PIDs, four broker cell paths with nonzero counters, UE's actual serving observation |
| N04 separate frequencies | Two fixed-frequency RF environments | Separate 3350/3400 brokers and UE observations; no scanning claim |
| N05 additional bandwidth | 25 PRB co-channel experiment | Effective 5.76 Msps allocation and the native UE cell-search log reporting PRB 25 |
| N06 isolated cores | Two independently isolated cores | Both chains attach; stop owned core 2 and show core 1 still attached |
| N08 templates | Preview, create, edit, save, capture and restore | Fresh project/component IDs, preserved snapshot and valid restored ownership/path references |
| N09 compatibility | Inspect the exact-profile label, then change an IMSI to duplicate another UE | Native-verified baseline changes to an explicit invalid design; restore a unique IMSI before starting |
| N07 blocked boundary | Select Open5GS as core adapter | Explicit missing-dependency/unfinished-execution diagnostics; no native success claimed |

The implementation report contains the plain-language flow and glossary. The RF viewer separates designed allocation, current applied native evidence and historical/stopped evidence. Its counters establish transport; the UE's own NAS/CSV establishes registration and serving observations.

## Create two independent UEs

Use **Two independent UEs** or **Two UEs selecting different cells** when published. Alternatively add a second UE in Network Builder. Creation assigns a distinct synthetic IMSI; duplicate IMSIs are rejected. Authentication keys/OPc are generated privately per UE, never edited in the browser.

For each UE:

- Select **Core owner** when there is more than one EPC.
- Set **Search DL EARFCN**, or select **Assigned radio environment**. Assignment is planned configuration, not an observation.
- Set optional per-cell attenuation in its inspector. These are reciprocal static gains for that UE, not geographical path loss.
- Inspect its own Runtime tab and the independent UE panel. NAS attachment is derived from that UE's native log. CSV `is_attached` means RRC connected and is not substituted for NAS registration.
- In **Native Measurements**, choose that UE as the measurement source.

The manifest allocates separate configs/logs/CSVs, sockets, native roles and TUN namespaces. All UEs are required participants in a shared RF matrix: one unexpected UE failure fails the experiment, stops healthy dependents as stopped, and retains the actual failed role. A healthy UE is not mislabeled the failed UE.

## Frequency and bandwidth choices

The legacy adapter retains direct single-cell and the original 50-PRB co-channel broker. `matrix-iq-v2` adds independent UE streams and carrier environments. **Automatic selection** chooses the matrix for extended shapes; select **legacy** explicitly to keep its original constraints. The registry refuses settings a selected adapter would ignore.

Implemented matrix rate profiles, based on this checkout's non-power-of-two LTE FFT sizes:

| PRBs | Nominal channel bandwidth | Native / matrix transport rate |
|---|---|---|
| 6 | 1.4 MHz | 1.92 Msps |
| 15 | 3 MHz | 3.84 Msps |
| 25 | 5 MHz | 5.76 Msps |
| 50 | 10 MHz | 11.52 Msps |
| 100 | 20 MHz | 23.04 Msps |

Code support is not native verification of every row. See the evidence matrix for tested combinations. 75 PRBs is not an implemented matrix profile. Legacy transport remains 23.04 Msps and the driver requires an integer transport/native-rate ratio. The old `config.base_srate` field remains a legacy input; matrix effective rates are derived from PRBs and recorded explicitly in the RF manifest and compatibility display.

Band 7 only: downlink `2620 + 0.1*(EARFCN-2750)` MHz, uplink 120 MHz below downlink, paired UL EARFCN = DL + 18000. Nominal bandwidth must fit inside Band 7. At 50 PRBs, 3350 and 3400 are two tested center frequencies.

Different frequencies use independent broker processes and fixed-frequency UEs. Basebands from different carrier centers are **not** summed. Same-carrier mixed bandwidth and mixed frequencies inside one eNodeB are rejected. Use separate eNodeBs and receivers per frequency. No inter-frequency scanning, retuning or handover is implied.

## Two core environments

Use **Two independently isolated cores** when published. Assign each eNodeB and UE to an EPC. Each environment has private core networking, its own subscriber file, and separate native processes; each UE has a private TUN namespace. Reusing a synthetic PLMN or private UE address in separate namespaces is not a collision and does not provide roaming.

The runtime offers **Stop owned core …** for matrix environments. It stops that core's UEs/brokers/eNodeBs/core only. The other environment remains running. Whole-chain restart still restarts the experiment; dynamic addition or per-core restart is not implemented.

## External core boundary

Selecting Open5GS currently produces an explicit blocked/unsupported diagnostic. No Open5GS binaries or MongoDB were available on the development host. Isolated subscriber provisioning, core configuration, lifecycle/readiness and attach acceptance remain unimplemented. Studio does not adopt or stop a pre-existing external core, and does not silently replace an Open5GS selection with srsEPC. Bundled srsEPC mode remains available.

Official references: [Open5GS quickstart](https://open5gs.org/open5gs/docs/guide/01-quickstart/) and [srsRAN S1 handover application note](https://docs.srsran.com/projects/4g/en/latest/app_notes/source/handover/source/index.html). Supporting an external core alone would not prove completed handover.

## Tests

Backend/protocol checks (use dedicated test data; broker prerequisite is built first):

```bash
roguard-studio/.venv/bin/python roguard-studio/scripts/build-broker.py
ROGUARD_REPO=/path/to/rogue-bs-detection \
  roguard-studio/.venv/bin/python -m pytest -c roguard-studio/pytest.ini roguard-studio/tests
npm --prefix roguard-studio/frontend run typecheck
npm --prefix roguard-studio/frontend run build
```

For native tests, use a disposable checkout/work directory with the same native binaries, not a live user's service database. `real_network.py` creates dedicated acceptance stores and unique run directories, cleans owned processes, and writes public result JSON separately from private generated configs:

```bash
roguard-studio/.venv/bin/python roguard-studio/tests/real_network.py \
  --templates single-cell sibling-cells two-enbs mixed-cells three-enbs \
              two-ues two-ues-two-enbs bandwidth-25 two-frequencies four-cells two-cores \
  --output /path/to/test-output/native-profiles.json

roguard-studio/.venv/bin/python roguard-studio/tests/real_network.py \
  --templates two-ues --kill-ue-2 --output /path/to/test-output/ue-failure.json

roguard-studio/.venv/bin/python roguard-studio/tests/real_network.py \
  --templates two-cores --stop-second-core --output /path/to/test-output/core-stop.json
```

For browser tests, the dedicated harness runs the **actual Studio app and actual Roguard app/Bridge/Store** in temporary databases. Its small dashboard proxy preserves the server-side credential boundary. It does not supply mock native runtime states. Use checked-free ports:

```bash
ROGUARD_REPO=/path/to/rogue-bs-detection \
  roguard-studio/.venv/bin/python roguard-studio/tests/studio_native_services.py \
  --data-dir /path/to/test-output/browser-state

# Another terminal, from frontend/:
ROGUARD_BACKEND_PORT=18280 ROGUARD_FRONTEND_PORT=13280 \
  npx next dev --webpack --hostname 127.0.0.1 --port 13280

STUDIO_TEST_URL=http://127.0.0.1:13280 \
ROGUARD_TEST_DASHBOARD=http://127.0.0.1:13282 npm run test:e2e
```

The native browser tests now complete the required connection dialog. The private harness credential file stays in its supplied test directory. Never copy it or generated HSS/UE secret files into documentation. See `docs/network-evidence/` for the checked-in sanitized execution manifest and results.

## Extending or troubleshooting

- `compatibility.py` is the common boundary for validation, generation, execution admission and template classification. Do not add a second permissive validator elsewhere.
- A model-valid design can be unsupported, missing prerequisites, or code-supported but unverified. Read the exact diagnostic.
- Native evidence is bound to canonical configuration, actual binary version/hashes and critical implementation SHA-256. Names/positions/new UUIDs do not change the radio profile; radio/subscriber/path/core edits do.
- An active broker stream is transport evidence, not proof of decoded cells. Read NAS logs and per-UE serving observations.
- Equal-strength co-channel interference can prevent attachment. Supported demonstrations use explicit attenuation and separate cold starts.
- Current serving correlation requires NAS evidence and a fresh UE CSV; ambiguous identities stay unresolved. The last assigned IP/RRC sample can remain historical after shutdown.
- Preserve existing evidence on failures. Inspect owned run logs and the manifest; do not reset databases, change host security policies or kill by process name.
