> Multi-cell execution has since been implemented and verified. See [native multi-cell runtime and current evidence](MULTICELL_RUNTIME.md). Earlier blocked-stage results below are retained as historical rebuild evidence.

# Owned-cell workbench validation — 31 October 2026

This report covers the owned-cell rebuild. Earlier acceptance JSON, screenshots and integration reports remain in place as historical evidence. No commit or deployment was made. Native binaries, RF plugin, source modifications and upstream configuration examples were preserved; all seven example-file hashes still match the original inspection manifest.

## Scope and results

| Requested stage | Result and evidence |
| --- | --- |
| 1. Preserve one EPC/eNodeB/UE baseline | Real native smoke passed: S1/NAS attachment, attached UE CSV and clean owned shutdown using existing binaries |
| 2. Frontend eNodeB with owned cell | Browser creates EPC/eNodeB/UE, expands/selects owned cells, edits native fields, saves and reloads without fabricated runtime state |
| 3. Generate native files | Generator uses authoritative `cells[].config`; browser previews PCI/RF mapping; unit tests cover root/paired UL and redaction; actual native runs consume generated files |
| 4. Execute multi-cell | **Blocked intentionally**: runner and generator reject unsupported multi-process/multi-cell shape; no partial first-cell execution |
| 5. Verify multiple running cells | **Not completed**; no native multi-cell measurement claim |
| 6. Integrate an I/Q broker | **Not implemented**; GNU Radio is absent and no flowgraph exists in the checkout; implementation plan documents protocol/rate/fan-out requirements |
| 7. Native serving-cell selection | Single-cell association verified from attached UE PCI/EARFCN and applied configuration; unknown/ambiguous/detached matches remain null |
| 8. Verify handover | **Not completed**; no handover fabricated from PCI changes or movement; bundled srsEPC also lacks S1 handover support |
| 9. Source evidence into Roguard bridge | Actual native PCI 2 measurements replayed through authenticated SSE into existing bridge normalization/persistence, using a temporary consumer database |
| 10. Tests/build/browser | Backend suite, TypeScript validation, production build and real browser suite run; commands and evidence below |

The host supports `/dev/net/tun` and unprivileged user/network/mount namespaces. The coding sandbox does not, so local socket/browser/native checks ran on the host with the established namespace isolation and existing executables. No host-root helper, CMake rebuild, system install or customer deployment was performed.

Final automated results: **38 backend tests passed; 3 browser tests passed; TypeScript validation and production build passed.** The browser suite includes a real LTE run/restart/export and a held-save-response regression that verifies later edits survive and canonical saved defaults clear the dirty flag.

## Evidence artifacts

- [Native API results](workbench-api-acceptance-results.json): saving a designed PCI change retains the current native PCI, restarting produces actual PCI 2, credential-redacted export contains real metrics, and restoration preserves the source snapshot.
- [Existing bridge replay](workbench-bridge-validation.json): source UUID, stable event ID, sequence, native measurement provenance, real attached-UE metrics and authenticated SSE status. The consumer database was temporary; the customer application was not edited/deployed.
- [Native installation audit](workbench-native-audit.json): installed binary/plugin identity and template preservation checks.
- [Desktop workbench](workbench-desktop.png) and [compact editor](workbench-compact.png): distinct new screenshots; original screenshots retained.
- [Capability matrix](CAPABILITIES.md), [native audit](NATIVE_AUDIT.md) and [specific multi-cell implementation plan](MULTICELL_PLAN.md).

Native process/run logs, generated configurations and CSV evidence remain in owned `roguard-studio/var/runs/` directories. Browser/API test topology/experiment persistence used a separate temporary Studio database. Do not interpret tests using protocol fixtures as live radio evidence; the native smoke, browser LTE test and API PCI-change test are the actual radio checks.

## Verification commands

```bash
roguard-studio/.venv/bin/python -m pytest -q -c roguard-studio/pytest.ini roguard-studio/tests
npm --prefix roguard-studio/frontend run typecheck
npm --prefix roguard-studio/frontend run build
roguard-studio/.venv/bin/python roguard-studio/tests/real_smoke.py
STUDIO_TEST_URL=http://127.0.0.1:53100 npm --prefix roguard-studio/frontend run test:e2e
STUDIO_TEST_API=http://127.0.0.1:53101 roguard-studio/.venv/bin/python roguard-studio/tests/real_api.py
# Pass the independent integration token through the environment, never in a report:
STUDIO_TEST_API=http://127.0.0.1:53101 roguard-studio/.venv/bin/python roguard-studio/tests/real_bridge_replay.py
```

The browser tests require running local Studio services. The real bridge replay expects the API acceptance evidence from the same isolated source and reads the existing independent Roguard backend via `ROGUARD_REPO` (default sibling repository). Its consumer database is created in a disposable temporary directory.

## Remaining limitations

The final target of one EPC, three running eNodeBs, at least three cells and one UE is **not operational**. Multiple configurations are designable and previewable, but execution remains gated. Neighbor/report adapters, I/Q summation/attenuation, multi-process supervision/measurement provenance, native handover confirmation and a capable core for S1 handover still require implementation and testing. Saved mobility paths have no RF effect. No measured coverage map, neighbor observation, RSRQ or calibrated position-derived radio data is claimed.

The backend test suite emits the existing Starlette/httpx deprecation warning. This does not fail tests and no dependency migration was included in this rebuild.

## Validation reporting correction

Follow-up regression checks: **40 backend tests and the targeted owned-cell browser test passed**. The report independently distinguishes a valid saved two-cell design, host readiness, and missing native adapter support. The browser verifies that both cells remain present, the host/topology checks pass on the ready test host, the adapter blocker remains explicit, and the two-cell native preview returns a design-only cell list without starting processes. Existing native LTE evidence above remains separate from these reporting tests.
