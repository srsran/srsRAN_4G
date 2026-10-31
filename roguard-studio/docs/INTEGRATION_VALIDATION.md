> Historical evidence retained. For the owned-cell rebuild and current execution limits, see [Workbench validation](WORKBENCH_VALIDATION.md).

# Integration verification — October 30, 2026

Implemented across `srsRAN_4G/roguard-studio` and the discovered sibling `rogue-bs-detection` repository. Existing uncommitted Studio work, native build artifacts, and Roguard simulation configuration changes were preserved. No commit or deployment was performed.

| Check | Result |
| --- | --- |
| Studio + Roguard backend pytest suites | 33 passed |
| Real local HTTP/SSE bridge restart/backfill | Included in backend suite; passed |
| Legacy Studio DB journal bootstrap | Included in backend suite; passed |
| Roguard frontend unit tests | 37 passed |
| TypeScript and production build | Passed; `/app/simulation` and its server API compiled |
| ESLint | Passed |
| Live/stale/disconnected/review browser tests | 4 passed, desktop and mobile |
| Real native LTE → bridge → customer browser → baseline approval | Passed with native PCI 1 then PCI 2 after full-chain restart |

The native test used actual srsEPC/srsENB/srsUE binaries, isolated ZeroMQ execution, native UE CSV telemetry, the authenticated source journal, persistent bridge ingestion, and the real Next.js customer portal in Chromium. It verified pending comparison against PCI 1 before approving PCI 2 through the browser. [Recorded native run and evidence references](roguard-native-demo-results.json).

Source observations and predictions were not fabricated. Prediction stayed null; only one native eNodeB/UE chain was exercised. Unit/browser fixture inputs are labeled as protocol/render fixtures and are distinct from this native result.

The native demonstration's temporary services/DBs were cleaned up; native run files remain in Studio's private ignored `var/runs/<run_id>` directories. The normal configured backend uses its persistent `backend/var` directory. Startup and reproduction commands are in [the integration guide](ROGUARD_INTEGRATION.md).

A Starlette test-client deprecation warning remains in the installed dependency set; it did not fail tests. This is a localhost research deployment with scoped service/analyst credentials, not a new public user/tenant authentication service. Observed topology validation covers the adapter's verified PCI/EARFCN fields; multi-cell execution and trained detection predictions remain unimplemented.

## Primary customer workspace update — October 31, 2026

Roguard `/app` now uses the native shared workspace and `/app/simulation` redirects there. The source adds safe applied graph identities and component IDs; source UUIDs, retained event payloads, durable replay, scoped read-only authentication and native single-chain behavior are preserved. The expanded real-native customer acceptance passed, including PCI 1 → PCI 2, browser approvals, mobile inspection and real bridge restart/backfill with 97 unique committed events. Final combined backend verification: 38 passed. See the sibling Roguard repository's `docs/customer-live-migration.md`, `docs/integration-validation.md` and `docs/customer-live-native-results.json`. Earlier validation evidence above is preserved.
