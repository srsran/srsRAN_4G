# srsRAN Studio workbench guide

Start `./roguard-studio/scripts/start.sh` from the repository root and use the printed URL. The launcher selects free local ports without stopping unrelated services. The browser uses the same-origin API/WebSocket proxy. The backend requires an ordinary-user Linux environment with TUN and unprivileged user/network/mount namespaces.

## Construct a network

Open **Network Builder**. The component library creates only real process roles: EPC, LTE eNodeB and UE. Clicking adds a process; dragging places it at a chosen canvas position. A new eNodeB owns one configured Cell automatically. The single-chain builder supplies S1 and reciprocal ZeroMQ transport relationships without a manual wiring ritual. Configured edges never count as attachment evidence.

Click an eNodeB to inspect its identity, cells, radio interfaces, core configuration, native logging and optional native AWGN. Bandwidth, RF device endpoints and process bindings belong to the eNodeB. Expand/collapse **Owned cells** on the node; click a child to configure its own radio identity. **Add Cell** is available on the eNodeB and its inspector. Two or three total co-channel cells can execute through the native I/Q broker; see [runtime limits and evidence](MULTICELL_RUNTIME.md).

Each cell has its own Cell ID, PCI, TAC, DL EARFCN, optional paired UL EARFCN, RF index and PRACH root sequence. Native field names, `rr.conf` location, default, supported range, dependency and restart impact are displayed. The current Band 7 adapter derives uplink if blank; otherwise UL must equal DL+18000. RF indices are allocated in order. Broker path attenuation controls real DL/UL sample scaling with a full restart. Handover remains blocked pending report/completion integration.

All process/cell identities are neutral. Identity, location or parameter edits never classify a base station as malicious. Experiment ground truth is captured separately; Roguard interprets native evidence.

## Review files and apply

The inspector's **Native files** tab previews the current design through the same generator used at startup. It shows native files, model-to-file trace, validation blockers and diffs against the last applied run. Credentials are redacted, temporary paths are represented by `<RUN>`, and generating a preview never alters applied files. Supported multi-cell previews show each actual native file and the explicit allocated broker RF map. Unsupported shapes show design-only lists.

**Save project** persists a design revision. **Validate** opens Simulation Runtime with three separate checks: Topology configuration, Runtime prerequisites (host only), and Native execution adapter. Supported two/three-cell co-channel designs pass the native-adapter check; other shapes remain blocked. **Start simulation** launches actual EPC/eNodeB/UE binaries only when all three checks pass. Direct mode preserves the single-cell shape; broker mode executes two/three co-channel cells across one to three eNodeBs with one EPC and UE. A valid-looking multi-node drawing does not bypass these limits.

Saving a supported PCI change while running retains the old native observation and marks restart required. Review the native diff and use **Restart chain** to stop the UE/eNodeB/EPC and start a new applied run. Pause and hot radio mutation are unavailable. Undo/redo affects the design; it does not roll back live radio state.

## Read evidence

- **Designed** topology: editable process ownership and configured relationships.
- **Applied** topology: configuration captured for the real run; current process states come from the owned supervisor. Cells share their eNodeB PID.
- **Observed** topology: native attached UE PCI/EARFCN and provenance. A unique applied-cell match supplies the serving-cell label; ECI is not independently measured, and ambiguous or detached associations stay unresolved.
- **Component Inventory**: designed processes and their owned configured cells, separately from runtime state.
- **Cell Configuration**: opens the builder with the first owned cell selected, then lets you select any other child.
- **ZeroMQ / RF Transport**: choose an eNodeB RF interface, review/edit designed ports, and inspect actual isolated runtime sockets separately. Adding transport edges is not I/Q summation.
- **Native Measurements**: actual CSV measurements and original units; missing values stay unavailable.
- **Logs & Diagnostics**: actual native and supervisor output with run provenance.

## Radio environment and mobility

**Radio Environment** is a 2D design-coordinate view. It displays eNodeBs, configured cells, UE placement and configured I/Q transport, plus a separate native serving-cell evidence panel. Positions are not a calibrated geographical map; no coverage heatmap or distance-derived RSRP is manufactured.

**Mobility Experiments** saves UE start/end points, planned speed and duration. A dashed planned path is labeled as having no RF effect. These are design controls only: no timeline changes samples or triggers handover without a future executable attenuation/channel adapter.

## Save experiments and integrate

**Capture experiment** saves the designed topology, applied run if available, native measurements, configuration and logs. Ground truth is explicitly analyst-supplied metadata. Export ZIP/JSON/CSV from **Experiment Manager**. Restore creates a stopped project with fresh component and cell identities; it preserves the original snapshot.

**Configuration Files** reopens saved projects and exports topology JSON. The owned-cell domain is `srsran.studio.topology.v2`; old unversioned flat projects convert on read. Old annotations remain non-executable. See **Capability Matrix** for the A–E support distinction and [the native audit](NATIVE_AUDIT.md) for source evidence.

Roguard integration remains read-only and authenticated. The existing token configuration and `/integration/v1/` snapshot, replay, SSE and schema routes remain in use. Read [the integration contract notes](ROGUARD_INTEGRATION.md) and [current verification](WORKBENCH_VALIDATION.md) before interpreting feature status.
