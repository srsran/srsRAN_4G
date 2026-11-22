# Advanced topology verification evidence

Execution date: 22 November 2026 (Africa/Cairo). Primary branch: `feat/roguard-lte-studio`; baseline commit `16f2eb9fa25ce3a0f20a440e961459a4749d3434`. The changes remain uncommitted. The earlier audit and its artifacts are preserved in the baseline commit.

## What the records establish

- [Native profiles](native-profiles.json): eleven real srsRAN 25.10.0 experiments, including the five preserved shapes, independent UEs, four cells, 25 PRBs, separate frequencies and two isolated cores. Each record contains topology, component IDs/PIDs, configuration hashes, binary version/hashes, implementation hash, run ID, readiness, actual NAS/serving observations, broker counters and short-run resources. Native cell-search/NAS excerpts corroborate the decoded PRB profile.
- [UE failure](ue-failure.json): actual UE-2 termination, failed-role identity, healthy-role shutdown and owned cleanup.
- [Core isolation](core-stop.json): actual scoped shutdown of core 2 while core 1 remains attached, followed by whole-run cleanup.
- The capability matrix is derived from these exact profile results. Template publication checks the fingerprint, native binaries/version, compiled broker and critical execution/evidence source hash. Full changed-file hashes in the evidence manifest additionally identify API, UI, tests and documentation.

The RF fixtures execute the compiled C++ broker against independent FC32 sources. They validate weighted downlink distribution, summed independent uplinks, frame boundaries and invalid manifests; they are protocol tests, not substitutes for native LTE acceptance.

## Final results and artifact index

| Check | Result | Evidence |
|---|---|---|
| Prepared baseline | 60 passed | [Baseline log](baseline-tests.log), [baseline metadata](baseline-metadata.json) |
| Final source backend/broker suite | 84 passed; one Starlette/httpx deprecation warning | [Log](backend-tests.log), [JUnit](backend-tests.xml) |
| Native acceptance | 11 profiles passed | [Profiles](native-profiles.json) |
| Actual fault/isolation tests | 2 passed | [UE-2 failure](ue-failure.json), [core-2 stop](core-stop.json) |
| Browser assertions | All 15 passed | [Final log](browser-tests.log), [recorded child exit](browser-exit.json) |
| TypeScript | Passed | [Log](typecheck.log) |
| Production build | Unchanged retry passed after installed-version advisory | [Initial](build-initial.log), [retry](build-retry.log) |
| Actual Roguard persistence | Native UE IDs retained; zero rejections | [Per-run summary](roguard-integration.json) |
| Authentication redaction | No exact private-value matches | [Scan scope/counts](redaction-check.json) |
| Workspace/service preservation | Checked separately from functional tests | [Preservation](preservation.json), [owned cleanup](cleanup.json) |

[Verification results](verification-results.json) record commands/statuses; [source file hashes](source-files.json) identify the complete changed source; [artifact hashes](evidence-manifest.json) identify the evidence package. [Host prerequisites](host-inventory.json), [toolchain](environment.json) and [compiled broker delivery](broker-delivery.json) make the execution context explicit.

The [native PCI-repair screenshot](screenshots/pci-repair-native-running.png) shows an actual running legacy regression. The [two-frequency RF screenshot](screenshots/two-frequency-restored-design.png) shows the **stopped, restored design**, not a claimed live attachment. Live acceptance comes from the native and browser records.

The first final browser execution reported `15 passed`, and its `.last-run.json` said `passed`, while the outer command session returned 143. The suite was repeated with a separate subprocess wrapper that records the Playwright child's own exit; see the final log and exit record. The first all-pass report is retained in [its own log](browser-all-pass-initial.log).

## Commands

Tests ran in a disposable copy under `/tmp/studio-network-20261122/runtime/srsRAN_4G`, with the existing native binaries copied byte-for-byte. Python came from the original Studio virtual environment. This avoided the user's SQLite stores and native run directories.

```bash
PYTHONDONTWRITEBYTECODE=1 ROGUARD_REPO=/path/to/rogue-bs-detection \
  /path/to/roguard-studio/.venv/bin/python -m pytest \
  -c pytest.ini tests --junitxml=/path/to/evidence/backend-tests.xml

/path/to/roguard-studio/.venv/bin/python tests/real_network.py \
  --templates single-cell sibling-cells two-enbs mixed-cells three-enbs \
              two-ues two-ues-two-enbs bandwidth-25 two-frequencies four-cells two-cores \
  --output /path/to/evidence/native-profiles.json

/path/to/roguard-studio/.venv/bin/python tests/real_network.py \
  --templates two-ues --kill-ue-2 --output /path/to/evidence/ue-failure.json
/path/to/roguard-studio/.venv/bin/python tests/real_network.py \
  --templates two-cores --stop-second-core --output /path/to/evidence/core-stop.json

# From frontend/; native service harness startup is in the developer guide.
npm run typecheck
npm run build
STUDIO_TEST_URL=http://127.0.0.1:13280 \
ROGUARD_TEST_DASHBOARD=http://127.0.0.1:13282 npm run test:e2e -- --reporter=line
```

The native suite writes results before export enrichment; the published manifest adds actual native PRB/NAS excerpts after checking every profile's fingerprint and current implementation hash. Private authentication configurations and databases are excluded.

## Preserved failures and limits

Development logs retain earlier failures separately from accepted results. Corrections included latching native S1 readiness across log-tail rotation, canonicalizing integer/float JSON encodings for exact-profile fingerprints, waiting for template navigation, and giving the PCI-repair test a unique project name in the retained test history. Advanced restore now remaps core, cell, environment and attenuation references and has three dedicated regression cases.

Next.js compiled the production application before stopping with `SecurityFatalError` and an installed-version security advisory. The same command succeeded on retry; both logs are retained. A development-server first invocation similarly stopped; its unchanged retry was verified with HTTP 200 before browser testing. No dependency upgrade or security-policy change was made. The advisory remains a dependency maintenance issue.

CPU/RSS records are short startup/attachment samples, not sustained throughput benchmarks. Native success covers the listed profiles only. No physical RF, inter-frequency scanning, handover, roaming or geographic propagation is claimed. N07 external-core execution is blocked: Open5GS/MongoDB are absent and isolated provisioning/lifecycle/attachment remain unimplemented.

The integration harness uses the actual Roguard FastAPI app, Bridge and Store on branch `feat/admin-command-foundation`, commit `43560e3f790af3f7f4eb3b0b3faae99c30f327b8`; its source was not modified. Its role-oriented presentation/detector scope still needs separate multi-UE consumer work.
