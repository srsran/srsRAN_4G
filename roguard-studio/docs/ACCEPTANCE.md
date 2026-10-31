> Historical evidence retained. For the owned-cell rebuild and current execution limits, see [Workbench validation](WORKBENCH_VALIDATION.md).

# Acceptance evidence — 30 October 2026

The installed binaries report srsRAN **25.10.0**, source commit `bef8680d5f9714f3e040e6f9cbc88d7888439b6d`. Host inspection confirmed ZeroMQ enabled, UHD disabled, TUN present and unprivileged user/network/mount namespaces functional. No existing LTE processes or standard active srsRAN config paths were found. The coding-agent filesystem sandbox hid TUN and prohibited namespaces; the real checks therefore ran on the host through reviewed tool execution, as the ordinary user.

No C++ files, upstream configuration examples, CMake settings or existing binaries were modified. The pre-existing untracked `build/` directory was preserved. Original example hashes and discovery details are recorded in [upstream-manifest.json](../configs/upstream-manifest.json).

## Results

| Requested acceptance criterion | Verification |
| --- | --- |
| 1. Open Studio locally | Next.js HTTP 200; actual Chrome browser tests against localhost |
| 2–4. Add EPC, eNodeB and UE visually | Browser component-library interactions; three actual React Flow nodes |
| 5. Connect valid components | Browser drags real React Flow handles for S1 and ZeroMQ; backend rejects invalid links |
| 6. Configure real srsRAN parameters | Strict config generator used by actual processes; API PCI mutation verified on a subsequent real run |
| 7. Save topology | Browser save/reload/reopen and persistent SQLite API tests |
| 8. Validate configuration | Browser prerequisites report; model/port/PLMN/multi-cell validation tests |
| 9. Start actual LTE | Browser starts existing EPC/eNB/UE in private namespaces |
| 10. See actual UE attachment | Real NAS attach-success output, S1 completion and UE CSV attachment evidence; IP `172.16.0.2` |
| 11. Monitor actual process state and metrics | Owned PID/status/RSS, UE CSV samples and established ZeroMQ sockets verified |
| 12. Stop and restart safely | Browser restart and stop; independent crash/recovery/EOF-cleanup tests |
| 13. Save scenario experiments | Browser records a synthetic suspicious event and captures an experiment snapshot |
| 14. Introduce a supported or synthetic change | Real PCI 1→2 change applied by restart; synthetic PLMN event did not alter actual observations |
| 15. Observe changes and export | Pre-authorized actual PCI change required explicit analyst approval; JSON/CSV/ZIP evidence exports verified |
| 16. Reopen without losing experiments | Backend persistence tests close/reopen the app against the same SQLite directory; browser reopens saved projects |

`tests/real_smoke.py` verified actual attachment, CSV observations and graceful shutdown. [reliability-results.json](reliability-results.json) records actual child crash detection, recovery attachment, cleanup on control-pipe EOF, unchanged host interfaces and preservation of an unrelated test process. [api-acceptance-results.json](api-acceptance-results.json) records the actual PCI change, unchanged applied state before restart, pre-authorization/approval, redacted export and non-destructive snapshot restore.

The API tests include synthetic parser fixtures strictly within tests. Product telemetry never uses those fixtures or fabricated chart values. Browser tests run against real backend services and native LTE binaries rather than route mocks.

## Automated checks

- 19 backend tests pass: graph/config constraints, local access controls, revision conflicts, private/redacted config generation, CSV partial-row/non-finite handling, persistent experiments and exports, baseline approval/pre-authorization, layout-independent restart calculation and non-destructive restore.
- Two Chrome/Playwright tests pass for real browser-to-LTE operation and editor history/grouping/responsive layout.
- TypeScript type checking and Next.js production build pass.
- Backend tests emit one dependency deprecation warning: the installed Starlette still supports the current HTTPX test client but recommends its newer test transport. It does not affect runtime API or the successful assertions.

## Remaining limits

These results establish the single-cell local adapter. They do **not** establish three-cell RF, handover, arbitrary multi-UE operation, a custom HSS editor, broad advanced SIB/RRC configuration, every possible LTE bandwidth/rate combination, or a Roguard detection model. [CAPABILITIES.md](CAPABILITIES.md) separates actual, restart-required, annotated, synthetic and unsupported operations. Gain/AWGN/JSON report fields are source-verified integrations; not every parameter combination has been independently exercised on real LTE.

## Source references

The isolated UE TUN separation follows the routing requirement described in the [official srsRAN ZeroMQ application note](https://docs.srsran.com/projects/4g/en/latest/app_notes/source/zeromq/source/index.html). The adapter's current option names and readiness messages were verified against the **local source tree**, which takes precedence over that older published documentation. Frontend custom node conventions follow [React Flow's custom node documentation](https://reactflow.dev/learn/customization/custom-nodes); WebSocket behavior follows [FastAPI's documentation](https://fastapi.tiangolo.com/advanced/websockets/).
