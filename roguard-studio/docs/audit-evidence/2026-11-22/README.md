# Studio implementation audit evidence — 22 November 2026

This directory contains **new evidence produced during the verification-only audit**, not copies of the original historical acceptance claims. See the [full report](../../../../STUDIO_IMPLEMENTATION_VERIFICATION_REPORT.md).

The audit used `/tmp/studio-audit-20261122/work/srsRAN_4G/roguard-studio` and independent temporary SQLite stores. Production source, original databases and existing services were preserved. The original Studio checkout advanced from `1f39982641b0a952f6afb1a74b0eb36c558630a4` to `e8b64044591cc247d2d5e7e0dd66a7794bd69b75` externally during the audit; the difference comprised the two initially modified frontend files. No auditor VCS mutation occurred. Roguard was inspected at `43560e3f790af3f7f4eb3b0b3faae99c30f327b8`, branch `feat/admin-command-foundation`, rather than expected `main`.

Results:

- Prepared Studio suite: **60 passed**. Initial fresh-copy host run: **59 passed, 1 failed** because a validation test required a broker binary before the broker fixture built it.
- Roguard bridge suite: **8 passed**.
- TypeScript check passed. Production build passed on unchanged retry after Next.js emitted a security diagnostic. The diagnostic was recorded, not resolved through an upgrade.
- Prepared Playwright suite: **6 passed, 3 failed**. Native-start tests stop at the mandatory Roguard connection dialog; screenshots and DOM contexts show the actual state.
- Native smoke, mandatory PCI save/restart, two/three eNodeBs, two/three sibling cells, mixed ownership, crash/restart/EOF, and actual bridge ingestion passed.
- Equal-gain three-eNodeB case **failed native attachment** despite active sample paths. This is a retained negative result, not a success.
- All audit-owned services/native processes stopped; the original port-8100 service remained listening.

## Evidence map

| Files | Meaning |
|---|---|
| `repository-metadata.json`, `source-manifest.json` | Repository identities and audited source SHA-256 hashes |
| `environment-host.txt` | Host prerequisites, native executable hashes, SCTP and namespace checks |
| `pytest*.log`, `pytest*.xml` | Initial restricted attempt, fresh-copy prerequisite failure, prepared 60-test pass |
| `roguard-pytest.log`, `roguard-pytest.xml` | Eight existing independent bridge tests |
| `typecheck.log`, `build*.log` | Frontend compile/build attempts and diagnostic |
| `playwright.log`, `playwright-ready.log` | Startup-failure attempt and prepared 6-pass/3-fail browser run |
| `browser-results/` | Actual native-workflow failure screenshots and DOM snapshots |
| `real-*.log`, `multi-*.log`, `probe-*.log` | Existing real-test stdout/stderr, including the equal-gain failure |
| `results/` | Fresh acceptance result JSON written by those tests in the disposable copy |
| `native-evidence-summary.json` | Extracted native NAS/S1 success, UE CSV fields and broker counters per run |
| `native/<run-id>/native-acceptance-lines.txt` | Selected actual EPC/S1/UE log lines, retaining original filename and line number |
| `native/<run-id>/*metrics.csv` | Actual native metrics, including unavailable `n/a` fields |
| `native/<run-id>/broker-counters.jsonl` | Actual compiled broker output; active samples do not imply decoded cells |
| `native/<run-id>/sanitized-configs/` | Actual generated native configuration with subscriber secrets redacted |
| `native/<run-id>/run-metadata.json` | Recorded run state/hashes plus current `.conf` hashes; direct probe metadata can be absent from service DB |
| `actual-multienb-export.json`, `.csv`, `.zip` | Actual native experiment exports, parsed and scanned for K/OPc/bearer exclusion |
| `audit-probes.json`, `audit-probes.log`, `audit_probes.py` | Verification-only counterexamples and checks: snapshot identity, CSV attribution, auth, persistence, RRC-only/stale fixtures |
| `start_api.py`, `start_native_api.py` | Dedicated audit-service launchers; refer to a private token file that is deliberately not shared |
| `summarize_native.py`, `package_evidence.py` | Evidence extraction/packaging scripts, not production implementation changes |
| `cleanup-result.json`, `cleanup.log`, `cleanup_audit.py` | Owned process cleanup and original-service preservation |
| `evidence-manifest.json` | SHA-256 of all shared evidence files except the manifest itself |

Native subscriber K/OPc, bearer credentials, private token files and live databases are **not included**. Packaging scanned files and uncompressed ZIP members against every temporary run's actual K/OPc and the temporary integration token. Synthetic subscriber identity and private temporary paths may appear as experiment metadata; they are not authentication keys.

The RRC-only and stale-observation cases are explicitly **fixtures**, not native LTE experiments. Binary FC32 tests also use fixture waveforms. Native LTE success is established separately from actual NAS/S1 logs and UE CSVs.

Scripts retain the exact temporary audit paths for reproducibility; they are evidence, not commands intended to be run against a user's active Studio. Do not replay cleanup or service scripts against other deployments. Original historical records remain in the parent `docs/` directory and were not overwritten.
