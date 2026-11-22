# Instructions

- Following Playwright test failed.
- Explain why, be concise, respect Playwright best practices.
- Provide a snippet of code with the fix, if possible.

# Test info

- Name: multicell.spec.ts >> two native eNodeBs attach through the real I/Q broker
- Location: tests/multicell.spec.ts:3:5

# Error details

```
Error: expect(locator).toBeVisible() failed

Locator: getByText('NAS evidence verified', { exact: true })
Expected: visible
Timeout: 110000ms
Error: element(s) not found

Call log:
  - Expect "toBeVisible" getByText('NAS evidence verified', { exact: true }) with timeout 110000ms
  - waiting for getByText('NAS evidence verified', { exact: true })

```

```yaml
- complementary:
  - link "srsRAN Studio":
    - /url: /
    - img
    - strong: srsRANSTUDIO
    - text: LTE SIMULATION PLATFORM
  - text: WORKSPACE
  - navigation:
    - button "Capability Matrix":
      - img
      - text: Capability Matrix
    - button "Network Builder":
      - img
      - text: Network Builder
    - button "Component Inventory":
      - img
      - text: Component Inventory
    - button "Simulation Runtime":
      - img
      - text: Simulation Runtime
    - button "Cell Configuration":
      - img
      - text: Cell Configuration
    - button "Radio Environment":
      - img
      - text: Radio Environment
    - button "Mobility Experiments":
      - img
      - text: Mobility Experiments
    - button "ZeroMQ / RF Transport":
      - img
      - text: ZeroMQ / RF Transport
    - button "Native Measurements":
      - img
      - text: Native Measurements
    - button "Logs & Diagnostics":
      - img
      - text: Logs & Diagnostics
    - button "Experiment Manager":
      - img
      - text: Experiment Manager
    - button "Configuration Files":
      - img
      - text: Configuration Files
    - button "Settings":
      - img
      - text: Settings
  - img
  - text: ISOLATED LABORATORY
  - paragraph: Synthetic subscribers ZeroMQ virtual radio
  - text: Local engine available
  - button "Collapse workspace":
    - img
    - text: Collapse workspace
  - text: RG LTE research Local workspace
- main:
  - button "CURRENT PROJECT Actual two-eNodeB broker verification":
    - img
    - text: CURRENT PROJECT
    - strong: Actual two-eNodeB broker verification
    - img
  - text: ZMQ NATIVE LTE
  - button "Engine connected"
  - text: 00:00:12
  - img
  - text: 22%
  - img
  - text: 71%
  - button "Save project":
    - img
    - text: Save project
  - button "Start simulation":
    - img
    - text: Start simulation
  - button "Stop owned LTE processes" [disabled]:
    - img
  - img
  - heading "Simulation Runtime" [level=1]
  - paragraph: srsRAN Studio · local LTE workbench
  - text: Simulation stopped
  - button "Capture experiment":
    - img
    - text: Capture experiment
  - text: SIMULATION RUNTIME
  - button "Undo · Ctrl+Z":
    - img
  - button "Redo · Ctrl+Shift+Z" [disabled]:
    - img
  - button "Validate":
    - img
    - text: Validate
  - button "Toggle inspector":
    - img
  - button "Toggle diagnostics":
    - img
  - text: PROCESS ORCHESTRATION
  - heading "Simulation control" [level=1]
  - paragraph: Verified readiness, owned process groups and explicit restart plans.
  - text: stopped
  - button "Validate topology":
    - img
    - text: Validate topology
  - button "Start LTE network":
    - img
    - text: Start LTE network
  - button "Restart chain":
    - img
    - text: Restart chain
  - button "Stop network" [disabled]:
    - img
    - text: Stop network
  - heading "Configuration & environment validation" [level=3]
  - img
  - text: Topology configuration Passed
  - img
  - text: Runtime prerequisites Passed
  - img
  - text: Native execution adapter Supported
  - heading "Applied process state" [level=3]
  - img
  - strong: Smoke EPC
  - paragraph: PID —
  - text: stopped
  - img
  - strong: Smoke eNB
  - paragraph: PID —
  - text: stopped
  - img
  - strong: Smoke UE
  - paragraph: PID —
  - text: stopped UE attachment Not evidenced
  - heading "Dependency restart plan RESTART REQUIRED" [level=3]
  - paragraph: The runner reconciles the complete dependency chain. Selected-component restart uses this same full-chain plan. Pause is unsupported.
  - list:
    - listitem: 01 Stop owned UE, then I/Q broker, all eNodeBs, and EPC
    - listitem: 02 Generate per-process native files and explicit RF plan
    - listitem: 03 Prepare owned user/network/mount namespaces
    - listitem: 04 Start EPC and verify MME/SPGW readiness
    - listitem: 05 Start all eNodeBs and the FC32 broker
    - listitem: 06 Start UE; require every eNodeB S1 response and actual NAS attachment
  - text: Radio changes are saved as design changes. The applied snapshot updates only when a new real run begins. Pause/resume are unsupported.
  - separator "Resize diagnostics"
  - img
  - button "All output"
  - button "eNB"
  - button "UE"
  - button "EPC"
  - button "ZeroMQ"
  - button "System events"
  - button "Validation"
  - textbox "Filter diagnostics":
    - /placeholder: Filter output…
  - text: 300 entries
  - button "Expand diagnostics":
    - img
  - button "Hide diagnostics":
    - img
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.152478 [PHY1 ] [I] [ 2860] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.156646 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.175066 [PHY1 ] [I] [ 2880] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.175761 [PHY1 ] [I] [ 2877] PUCCH: cc=0, tti_tx=2881, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.176204 [PHY0 ] [I] [ 2881] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.179125 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.197147 [PHY1 ] [I] [ 2900] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.201444 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.219606 [PHY1 ] [I] [ 2920] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.220452 [PHY1 ] [I] [ 2917] PUCCH: cc=0, tti_tx=2921, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.220722 [PHY0 ] [I] [ 2921] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.223696 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.242005 [PHY1 ] [I] [ 2940] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.246218 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.264447 [PHY1 ] [I] [ 2960] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.265282 [PHY1 ] [I] [ 2957] PUCCH: cc=0, tti_tx=2961, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.265611 [PHY0 ] [I] [ 2961] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.268696 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.286566 [PHY1 ] [I] [ 2980] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.289769 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.305504 [MAC ] [I] [ 2993] BSR: triggered_bsr_type=Periodic, LCID QUEUE status: 0: 0 1: 0 2: 0 3: 0"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.308993 [PHY1 ] [I] [ 3000] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.310011 [PHY1 ] [I] [ 2997] PUCCH: cc=0, tti_tx=3001, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.310322 [PHY0 ] [I] [ 3001] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.313201 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.331487 [PHY1 ] [I] [ 3020] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.335784 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.353734 [PHY1 ] [I] [ 3040] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.354679 [PHY1 ] [I] [ 3037] PUCCH: cc=0, tti_tx=3041, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.354930 [PHY0 ] [I] [ 3041] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.357916 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.376853 [PHY1 ] [I] [ 3060] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.380722 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.399193 [PHY1 ] [I] [ 3080] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.400414 [PHY1 ] [I] [ 3077] PUCCH: cc=0, tti_tx=3081, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.400884 [PHY0 ] [I] [ 3081] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.403299 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.421605 [PHY1 ] [I] [ 3100] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.424706 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.444038 [PHY1 ] [I] [ 3120] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.444978 [PHY1 ] [I] [ 3117] PUCCH: cc=0, tti_tx=3121, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.445255 [PHY0 ] [I] [ 3121] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.448304 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.466622 [PHY1 ] [I] [ 3140] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.470734 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.478595 [STCK ] [I] tti_tprof: {mean, max, min} = {0.01, 0, 0} msec"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.489081 [PHY1 ] [I] [ 3160] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.489980 [PHY1 ] [I] [ 3157] PUCCH: cc=0, tti_tx=3161, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.490309 [PHY0 ] [I] [ 3161] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.493357 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.512021 [PHY1 ] [I] [ 3180] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.516386 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.534519 [PHY1 ] [I] [ 3200] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.535504 [PHY1 ] [I] [ 3197] PUCCH: cc=0, tti_tx=3201, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.535883 [PHY0 ] [I] [ 3201] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.538818 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.557137 [PHY1 ] [I] [ 3220] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.561417 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.579785 [PHY1 ] [I] [ 3240] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.580719 [PHY1 ] [I] [ 3237] PUCCH: cc=0, tti_tx=3241, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.581127 [PHY0 ] [I] [ 3241] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.584212 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.602724 [PHY1 ] [I] [ 3260] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.605760 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.625044 [PHY1 ] [I] [ 3280] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.626014 [PHY1 ] [I] [ 3277] PUCCH: cc=0, tti_tx=3281, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.626491 [PHY0 ] [I] [ 3281] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.629249 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.647675 [PHY1 ] [I] [ 3300] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.651936 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.670352 [PHY1 ] [I] [ 3320] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.671215 [PHY1 ] [I] [ 3317] PUCCH: cc=0, tti_tx=3321, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.671549 [PHY0 ] [I] [ 3321] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.674658 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.693048 [PHY1 ] [I] [ 3340] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.696047 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.715274 [PHY1 ] [I] [ 3360] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.716252 [PHY1 ] [I] [ 3357] PUCCH: cc=0, tti_tx=3361, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.716578 [PHY0 ] [I] [ 3361] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.718421 [NAS ] [I] Switching off
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.718422 [NAS ] [I] Sending detach request with GUTI
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.718497 [RRC ] [I] SRB2 - Tx ulInformationTransfer (24 B)
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.718505 [PDCP ] [I] TX SRB2 PDU, SN=1, integrity=tx/rx, encryption=tx/rx
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.718507 [RLC ] [I] SRB2: Tx SDU (29 B, PDCP_SN=1 tx_sdu_queue_len=1)"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.718515 [MAC ] [I] [ 3359] SR: Signalling PHY sr_counter=1"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.718521 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.737597 [PHY0 ] [I] [ 3376] PUCCH: cc=0, tti_tx=3380, rnti=0x46, f=1, n_pucch=0, sr=yes"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.737917 [PHY1 ] [I] [ 3380] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, sr=yes, ta=0.0 us"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.737943 [MAC ] [I] [ 3380] SCHED: UL tx rnti=0x46, cc=0, pid=4, dci=(0,4), prb=[2, 11), n_rtx=0, cfi=1, tbs=517, bsr=0 (512-517), tti_tx_ul=3388"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.737973 [PHY1 ] [I] [ 3380] PDCCH: cc=0, rnti=0x46, f=0, cce= 4, L=0, riv=402, mcs=22, rv=0, ndi=0, f_h=n/a, cqi=no, tpc_pusch=1, dmrs_cs=0, tti_tx_dl=3384"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.742039 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.746500 [PHY0 ] [I] [ 3384] PDCCH: cc=0, f=0, cce= 4, L=0, riv=402, mcs=22, rv=0, ndi=0, f_h=n/a, cqi=no, tpc_pusch=1, dmrs_cs=0, snr=143.4 dB"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.746540 [RLC ] [I] SRB2: Tx PDU SN=1 (31 B)"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.746547 [MAC ] [I] [ 3384] UL PHR: ph=30.0 LCID=2 len=31 LBSR: b=0 0 0 0"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.746549 [MAC ] [I] [ 3384] UL 4: New TX, RV=0, TBS=517"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.746670 [PHY0 ] [I] [ 3384] PUSCH: cc=0, tti_tx=3388, rnti=0x46, rb=(2,10), nof_re=1296, tbs=517, mod=4, rv=0"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.747080 [MAC ] [I] [ 3388] SCHED: UL adaptive link: rnti=0x46, snr_estim=111.42, last_mcs=22, snr_offset=0.012000"
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.747081 [MAC ] [I] [ 3388] Pushing PDU rnti=0x46, tti_rx=3388, nof_bytes=517
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.747088 [PHY1 ] [I] [ 3388] PUSCH: cc=0, rnti=0x46, rb=(2,10), nof_re=1296, tbs=517, mod=4, rv=0, crc=OK, avg_iter=2.0, snr=111.4 dB, epre=142.8 dBfs, ta=0.0 us, cfo=-0.0 hz, t=110 us"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.747113 [PHY1 ] [I] [ 3388] PHICH: rnti=0x46, hi=1, I_lowest=2, n_dmrs=0, tti_tx_dl=3392"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.747127 [MAC ] [I] [ 3388] 0x46 UL PHR: ph=30.0 LBSR: b=0 0 0 0 LCID=2 len=31 PAD: len=477"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.747131 [RLC ] [I] SRB2: Rx PDU - N bytes 31"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.747132 [RLC ] [I] SRB2: Rx data PDU SN=1 (29 B)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.747134 [RLC ] [I] SRB2: Status packet requested through polling bit"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.747135 [RLC ] [I] SRB2: Rx SDU (29 B)"
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.747137 [PDCP ] [I] SRB2 Rx PDU SN=1 (29 B, integrity=tx/rx, encryption=tx/rx)
  - time: 17:20:38
  - text: "epc 2026-11-22T15:20:38.748157 [S1AP ] [I] Received S1AP msg. Size: 67"
  - time: 17:20:38
  - text: epc 2026-11-22T15:20:38.748170 [S1AP ] [I] Received Initiating PDU
  - time: 17:20:38
  - text: epc 2026-11-22T15:20:38.748170 [S1AP ] [I] Received Uplink NAS Transport Message.
  - time: 17:20:38
  - text: "epc UL NAS: Detach Request"
  - time: 17:20:38
  - text: "epc 2026-11-22T15:20:38.748221 [NAS ] [I] Integrity check ok. Local: count=3, Received: count=3"
  - time: 17:20:38
  - text: epc Detach request -- IMSI 001010123456780
  - time: 17:20:38
  - text: "epc 2026-11-22T15:20:38.748222 [S1AP ] [I] UL NAS: sec_hdr_type: LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY, mac_vaild: yes, msg_encrypted: no"
  - time: 17:20:38
  - text: "epc 2026-11-22T15:20:38.748223 [S1AP ] [I] UL NAS: Detach Request"
  - time: 17:20:38
  - text: epc 2026-11-22T15:20:38.748229 [NAS ] [I] Detach request -- IMSI 001010123456780
  - time: 17:20:38
  - text: epc 2026-11-22T15:20:38.748230 [MME GTPC] [I] Sending GTP-C Delete Session Request request. IMSI 1010123456780
  - time: 17:20:38
  - text: epc 2026-11-22T15:20:38.748230 [MME GTPC] [I] GTP-C Delete Session Request -- S-GW Control TEID 1
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.748082 [MAC ] [I] [ 3389] SCHED: DL tx rnti=0x46, cc=0, pid=0, mask=0x10000, dci=(0, 7), n_rtx=0, cfi=1, tbs=7, buffer=7/0, tti_tx_dl=3393"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.748092 [RLC ] [I] Tx status PDU - SRB2: ACK_SN = 2, N_nack = 0"
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.748098 [MAC ] [I] [ 3389] 0x46 DL LCID=2 len=2
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.748101 [RRC ] [I] Rx SRB2 PDU, rnti=0x46 - ulInformationTransfer (24 B)
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.748103 [S1AP ] [I] Received RRC SDU
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.748117 [S1AP ] [I] Tx S1AP SDU, UplinkNASTransport, rnti=0x46
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.748125 [PHY0 ] [I] [ 3389] PDCCH: cc=0, rnti=0x46, f=1, cce= 7, L=0, rbg=0x10000, pid=0, mcs={0}, ndi={0}, tpc_pucch=1, tti_tx_dl=3393"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.748150 [PHY0 ] [I] [ 3389] PDSCH: cc=0, rnti=0x46, nof_prb=3, nof_re=450, tbs={7}, mod={2}, rv={0}, tti_tx_dl=3393"
  - time: 17:20:38
  - text: "epc Received GTP-C PDU. Message type: GTPC_MSG_TYPE_DELETE_SESSION_REQUEST"
  - time: 17:20:38
  - text: epc Received UE Context Release Complete. MME-UE S1AP Id 1
  - time: 17:20:38
  - text: epc UE Context Release Completed.
  - time: 17:20:38
  - text: "epc 2026-11-22T15:20:38.748364 [S1AP ] [I] Received S1AP msg. Size: 19"
  - time: 17:20:38
  - text: epc 2026-11-22T15:20:38.748372 [S1AP ] [I] Received Succeseful Outcome PDU
  - time: 17:20:38
  - text: epc 2026-11-22T15:20:38.748373 [S1AP ] [I] Received UE Context Release Complete
  - time: 17:20:38
  - text: "epc 2026-11-22T15:20:38.748373 [S1AP ] [I] Received UE Context Release Complete. MME-UE S1AP Id: 1"
  - time: 17:20:38
  - text: epc 2026-11-22T15:20:38.748377 [S1AP ] [I] Released UE ECM Context.
  - time: 17:20:38
  - text: epc 2026-11-22T15:20:38.748377 [S1AP ] [I] UE Context Release Completed.
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.748311 [S1AP ] [I] Rx S1AP SDU - UEContextReleaseCommand
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.748313 [S1AP ] [I] Checking UE S1 logical connection. eNB UE S1AP ID=1, MME UE S1AP ID=1
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.748314 [RRC ] [I] Releasing E-RABs for 0x46
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.748317 [GTPU ] [I] Removed rnti=0x46,eps-BearerID=5 tunnel with TEID In=0x1
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.748332 [S1AP ] [I] Tx S1AP SDU, UEContextReleaseComplete, rnti=0x46
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.748364 [S1AP ] [I] UE context for RNTI:0x46 released
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.749095 [RRC ] [I] Received Release Complete rnti=0x46
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.749104 [RLC ] [I] Cleared buffer rnti=0x46
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.749114 [RRC ] [I] Tx SRB1 PDU, rnti=0x46 - rrcConnectionRelease (2 B)
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.749127 [PDCP ] [I] TX SRB1 PDU, SN=6, integrity=tx/rx, encryption=tx/rx
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.749129 [RLC ] [I] SRB1: Tx SDU (7 B, PDCP_SN=6 tx_sdu_queue_len=1)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.749171 [MAC ] [I] [ 3390] SCHED: DL tx rnti=0x46, cc=0, pid=1, mask=0x10000, dci=(0, 4), n_rtx=0, cfi=1, tbs=18, buffer=14/0, tti_tx_dl=3394"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.749178 [RLC ] [I] SRB1: Tx PDU SN=6 (9 B)"
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.749181 [MAC ] [I] [ 3390] 0x46 DL LCID=1 len=9
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.749199 [PHY1 ] [I] [ 3390] PDCCH: cc=0, rnti=0x46, f=1, cce= 4, L=0, rbg=0x10000, pid=1, mcs={2}, ndi={0}, tpc_pucch=1, tti_tx_dl=3394"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.749215 [PHY1 ] [I] [ 3390] PDSCH: cc=0, rnti=0x46, nof_prb=3, nof_re=450, tbs={18}, mod={2}, rv={0}, tti_tx_dl=3394"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.755532 [PHY0 ] [I] [ 3392] PHICH: hi=1, corr=1.0, I_lowest=2, n_dmrs=0, I_phich=0"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.756670 [PHY1 ] [I] [ 3393] PDCCH: cc=0, f=1, cce= 7, L=0, rbg=0x10000, pid=0, mcs={0}, ndi={0}, tpc_pucch=1, snr=142.8 dB"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.756694 [PHY1 ] [I] [ 3393] PDSCH: cc=0, rnti=0x46, nof_prb=3, nof_re=450, tbs={7}, mod={2}, rv={0}, crc={OK}, it=1.0, snr=142.8 dB"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.756698 [MAC ] [I] [ 3393] DL 0 (TB 0): newTX tbs=7, rv=0, ack=OK, ndi=0"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.756770 [PHY1 ] [I] [ 3393] PUCCH: cc=0, tti_tx=3397, rnti=0x46, f=1a, n_pucch=19, ack=1"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.756788 [MAC ] [I] [ 3393] DL LCID=2 len=2 PAD: len=2"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.756790 [RLC ] [I] SRB2: Rx PDU - N bytes 2"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.756793 [RLC ] [I] Rx Status PDU SRB2: ACK_SN = 2, N_nack = 0"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.757063 [MAC ] [I] [ 3397] SCHED: DL adaptive link: rnti=0x46, cqi=15, last_mcs=0, cqi_offset=0.017000"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.757069 [PHY0 ] [I] [ 3397] PUCCH: cc=0; rnti=0x46, f=1a, n_pucch=19, dmrs_corr=1.000, snr=111.7 dB, corr=1.000, ack=1, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.757790 [PHY0 ] [I] [ 3394] PDCCH: cc=0, f=1, cce= 4, L=0, rbg=0x10000, pid=1, mcs={2}, ndi={0}, tpc_pucch=1, snr=142.9 dB"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.757821 [PHY0 ] [I] [ 3394] PDSCH: cc=0, rnti=0x46, nof_prb=3, nof_re=450, tbs={18}, mod={2}, rv={0}, crc={OK}, it=1.0, snr=142.9 dB"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.757825 [MAC ] [I] [ 3394] DL 1 (TB 0): newTX tbs=18, rv=0, ack=OK, ndi=0"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.757891 [PHY0 ] [I] [ 3394] PUCCH: cc=0, tti_tx=3398, rnti=0x46, f=1a, n_pucch=16, ack=1"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.757916 [MAC ] [I] [ 3394] DL LCID=1 len=9 PAD: len=6"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.757917 [RLC ] [I] SRB1: Rx PDU - N bytes 9"
  - time: 17:20:38
  - text: "ue Received RRC Connection Release (releaseCause: other)"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.757919 [RLC ] [I] SRB1: Rx data PDU SN=6 (7 B)"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.757921 [RLC ] [I] SRB1: Status packet requested through polling bit"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.757922 [RLC ] [I] SRB1: Rx SDU (7 B)"
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.757924 [PDCP ] [I] SRB1 Rx PDU SN=6 (7 B, integrity=tx/rx, encryption=tx/rx)
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.757936 [RRC ] [I] SRB1 - Rx rrcConnectionRelease (2 B)
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.758188 [MAC ] [I] [ 3398] SCHED: DL adaptive link: rnti=0x46, cqi=15, last_mcs=2, cqi_offset=0.018000"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.758193 [PHY1 ] [I] [ 3398] PUCCH: cc=0; rnti=0x46, f=1a, n_pucch=16, dmrs_corr=1.000, snr=111.7 dB, corr=1.000, ack=1, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.758906 [MAC ] [I] [ 3395] SR: Signalling PHY sr_counter=1"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.760123 [PHY0 ] [I] [ 3396] PUCCH: cc=0, tti_tx=3400, rnti=0x46, f=1, n_pucch=0, sr=yes"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.760433 [PHY1 ] [I] [ 3400] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, sr=yes, ta=0.0 us"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.760461 [MAC ] [I] [ 3400] SCHED: UL tx rnti=0x46, cc=0, pid=0, dci=(0,4), prb=[2, 11), n_rtx=0, cfi=1, tbs=517, bsr=0 (512-517), tti_tx_ul=3408"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.760483 [PHY1 ] [I] [ 3400] PDCCH: cc=0, rnti=0x46, f=0, cce= 4, L=0, riv=402, mcs=22, rv=0, ndi=1, f_h=n/a, cqi=no, tpc_pusch=1, dmrs_cs=0, tti_tx_dl=3404"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.761327 [PHY1 ] [I] [ 3397] PUCCH: cc=0, tti_tx=3401, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.761658 [PHY0 ] [I] [ 3401] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.764601 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.769251 [PHY0 ] [I] [ 3404] PDCCH: cc=0, f=0, cce= 4, L=0, riv=402, mcs=22, rv=0, ndi=1, f_h=n/a, cqi=no, tpc_pusch=1, dmrs_cs=0, snr=143.8 dB"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.769286 [RLC ] [I] Tx status PDU - SRB1: ACK_SN = 7, N_nack = 0"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.769318 [MAC ] [I] [ 3404] UL LCID=1 len=2 LBSR: b=0 0 0 0"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.769320 [MAC ] [I] [ 3404] UL 0: New TX, RV=0, TBS=517"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.769442 [PHY0 ] [I] [ 3404] PUSCH: cc=0, tti_tx=3408, rnti=0x46, rb=(2,10), nof_re=1296, tbs=517, mod=4, rv=0"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.769870 [MAC ] [I] [ 3408] SCHED: UL adaptive link: rnti=0x46, snr_estim=111.42, last_mcs=22, snr_offset=0.013000"
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.769872 [MAC ] [I] [ 3408] Pushing PDU rnti=0x46, tti_rx=3408, nof_bytes=517
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.769880 [PHY1 ] [I] [ 3408] PUSCH: cc=0, rnti=0x46, rb=(2,10), nof_re=1296, tbs=517, mod=4, rv=0, crc=OK, avg_iter=2.0, snr=111.4 dB, epre=142.8 dBfs, ta=0.0 us, cfo=-0.0 hz, t=86 us"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.769914 [PHY1 ] [I] [ 3408] PHICH: rnti=0x46, hi=1, I_lowest=2, n_dmrs=0, tti_tx_dl=3412"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.769983 [MAC ] [I] [ 3408] 0x46 UL LBSR: b=0 0 0 0 LCID=1 len=2 PAD: len=508"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.769986 [RLC ] [I] SRB1: Rx PDU - N bytes 2"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.769991 [RLC ] [I] Rx Status PDU SRB1: ACK_SN = 7, N_nack = 0"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.778520 [PHY0 ] [I] [ 3412] PHICH: hi=1, corr=1.0, I_lowest=2, n_dmrs=0, I_phich=0"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.783191 [PHY1 ] [I] [ 3420] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.787337 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.805721 [PHY1 ] [I] [ 3440] PUCCH: cc=0; rnti=0x46, f=1, n_pucch=0, dmrs_corr=-nan, snr=-inf dB, corr=0.000, sr=no, ta=0.0 us"
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.805742 [GTPU ] [I] Removing rnti=0x46
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.805743 [STCK ] [I] Bearers: Removed rnti=0x46 from EPS bearer manager"
  - time: 17:20:38
  - text: enb Disconnecting rnti=0x46.
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.805958 [RRC ] [I] Deallocated CQI resources for time-frequency slot (0, 0)
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.805959 [RRC ] [I] Deallocated SR resources for time-frequency slot (0, 0)
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.805972 [RRC ] [I] Removed user rnti=0x46
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.806631 [PHY1 ] [I] [ 3437] PUCCH: cc=0, tti_tx=3441, rnti=0x46, f=2, n_pucch=0, cqi=15 (cc=0)"
  - time: 17:20:38
  - text: "enb 2026-11-22T15:20:38.806931 [PHY0 ] [I] [ 3441] PUCCH: cc=0; rnti=0x46, f=2, n_pucch=0, dmrs_corr=1.000, snr=111.8 dB, corr=1.000, cqi=15 (cc=0), ta=0.0 us"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.809989 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: enb 2026-11-22T15:20:38.817513 [MAC ] [I] [ 3450] User rnti=0x46 removed from MAC/PHY
  - time: 17:20:38
  - text: ue RRC IDLE
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.825818 [RRC ] [I] Proc "Go Idle" - Starting...
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.825828 [RRC ] [I] Leaving RRC_CONNECTED state
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.825830 [MAC ] [I] [ 3454] Resetting MAC
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.825831 [RRC ] [I] Setting default PHY config dedicated
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.825831 [PHY ] [I] [ 3454] Setting configuration
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.825837 [MAC ] [I] [ 3454] Setting SR configuration
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.825843 [MAC ] [I] [ 3454] Logical Channel Setup: LCID=0, LCG=0, priority=0, PBR=-1, BSD=50ms, bucket_size=0"
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.825845 [PHY ] [I] [ 3454] Setting new PHY configuration cc_idx=0...
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.825846 [MAC ] [I] [ 3454] Logical Channel Setup: LCID=1, LCG=0, priority=1, PBR=-1, BSD=50ms, bucket_size=0"
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.826034 [RLC ] [I] Added LTE radio bearer with LCID 0 in Transparent Mode
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.826038 [RRC ] [I] Setting MAC default configuration
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.826038 [MAC ] [I] [ 3454] Setting configuration
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.826039 [MAC ] [I] [ 3454] BSR: Configured timer reTX 2560 ms"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.826041 [RRC ] [I] Set MAC main config: harq-MaxReTX=5, bsr-TimerReTX=2560, bsr-TimerPeriodic=-1, SR disabled (dsr-TransMax=0)"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.826042 [STCK ] [I] Bearers: Reset EPS bearer manager"
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.826043 [RRC ] [I] Going RRC_IDLE
  - time: 17:20:38
  - text: ue 2026-11-22T15:20:38.826044 [RRC ] [I] Proc "Go Idle" - Left connected state
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.832821 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.855207 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.878085 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.901555 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.924121 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.946813 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.968918 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:38
  - text: "ue 2026-11-22T15:20:38.991280 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.012429 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.034585 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.058018 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.080221 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.102577 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.124046 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.147587 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.168971 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.192533 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.214128 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.237602 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.258962 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.282447 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.303581 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.326985 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.348205 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.370757 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.394296 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.415487 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.429988 [MAC ] [I] [ 3992] BSR: triggered_bsr_type=none, LCID QUEUE status: 0: 0 1: 0 2: 0 3: 0"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.439163 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.461687 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.482750 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.506160 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.527262 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.550658 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.572920 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.595043 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.617215 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.629753 [STCK ] [I] tti_tprof: {mean, max, min} = {0.01, 0, 0} msec"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.639691 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.662896 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.683894 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.707221 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.729426 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.751787 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.773915 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.795195 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.817419 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.839702 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.861803 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.885185 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.907758 [RRC ] [I] MEAS: New measurement serving cell: earfcn=0, pci=2, rsrp=62.70 dBm, cfo=+0.0 Hz"
  - time: 17:20:39
  - text: "ue 2026-11-22T15:20:39.911725 [NAS ] [I] Saved GUTI to file m_tmsi: 421c6246, mcc: 1, mnc: 1, mme_group_id: 1, mme_code: 1a"
  - time: 17:20:39
  - text: ue [authentication material redacted]
  - time: 17:20:42
  - text: ue Couldn't stop after 5s. Forcing exit.
  - time: 17:20:42
  - text: ue Saving NAS PCAP file (DLT=148) to
  - time: 17:20:42
  - text: enb Stopping ..
  - time: 17:20:45
  - text: epc 2026-11-22T15:20:45.666824 [GTPU ] [I] IPv6 not supported yet.
  - time: 17:20:45
  - text: "enb 2026-11-22T15:20:45.918470 [COMN ] [D] [ 0] RxSockets: Closing rx socket handler thread"
  - time: 17:20:45
  - text: "enb 2026-11-22T15:20:45.918507 [COMN ] [D] [ 0] RxSockets: closed."
  - time: 17:20:45
  - text: "epc SCTP Association Shutdown. Association: 0"
  - time: 17:20:45
  - text: "epc 2026-11-22T15:20:45.918548 [S1AP ] [I] SCTP Association Shutdown. Association: 0"
  - time: 17:20:45
  - text: "epc 2026-11-22T15:20:45.918554 [S1AP ] [E] Could not find eNB to delete. Association: 0"
  - time: 17:20:45
  - text: enb --- exiting ---
  - time: 17:20:45
  - text: enb 2026-11-22T15:20:45.918550 [COMN ] [D] [ 0] Socket=6 was closed.
  - time: 17:20:45
  - text: enb 2026-11-22T15:20:45.918669 [COMN ] [D] [ 0] Socket=-1 could not be closed.
  - time: 17:20:45
  - text: enb 2026-11-22T15:20:45.919189 [COMN ] [D] [ 0] Socket=-1 could not be closed.
  - time: 17:20:45
  - text: epc Stopping ..
  - time: 17:20:45
  - text: "epc Deleting eNB context. eNB Id: 0x19b"
  - time: 17:20:45
  - text: "epc Deleting UE EMM context. IMSI: 001010123456780"
  - time: 17:20:45
  - text: "epc 2026-11-22T15:20:45.947752 [S1AP ] [I] Deleting eNB context. eNB Id: 0x19b"
  - time: 17:20:45
  - text: "epc 2026-11-22T15:20:45.947755 [S1AP ] [I] Deleting UE EMM context. IMSI: 001010123456780"
  - time: 17:20:45
  - text: epc
  - time: 17:20:45
  - text: epc --- exiting ---
  - time: 17:20:45
  - text: "epc 2026-11-22T15:20:45.956035 [HSS ] [I] Opened DB file: /tmp/studio-audit-20261122/work/srsRAN_4G/roguard-studio/var/runs/ea298efb-5010-46de-9803-72e5a9f25e43/user_db.csv srsRAN LTE Simulation Studio Local control API /api · local proxy 2D topology Native I/Q engine adapter 4 NODES / 4 LINKS"
- dialog "Connect RoGuard before simulation":
  - heading "Connect RoGuard before simulation" [level=2]
  - heading "RoGuard connection" [level=3]
  - paragraph: Connect RoGuard to this simulation using your existing local credentials.
  - text: RoGuard dashboard URL
  - textbox "RoGuard dashboard URL":
    - /placeholder: http://localhost:3000
    - text: http://localhost:3000
  - text: Studio API URL
  - textbox "Studio API URL":
    - /placeholder: http://127.0.0.1:8000
    - text: http://127.0.0.1:18180
  - paragraph: Use the RoGuard dashboard’s port and Studio’s control API port. Both services must be running. Settings are saved and the bridge reconnects automatically.
  - status: "RoGuard: not-configured"
  - button "Cancel"
  - button "Connect & start simulation":
    - img
    - text: Connect & start simulation
- alert
```

# Test source

```ts
  1   | import { test, expect } from "@playwright/test";
  2   | 
  3   | test("two native eNodeBs attach through the real I/Q broker", async ({
  4   |   page,
  5   | }) => {
  6   |   test.setTimeout(150000);
  7   |   const errors: string[] = [];
  8   |   page.on("pageerror", (e) => errors.push(e.message));
  9   |   await page.goto("/");
  10  |   await expect(
  11  |     page.getByText("Engine connected", { exact: true }),
  12  |   ).toBeVisible();
  13  |   await page.locator(".project-switcher>button").click();
  14  |   await page.getByRole("button", { name: "New project", exact: true }).click();
  15  |   await page
  16  |     .getByLabel("Project name", { exact: true })
  17  |     .fill("Actual two-eNodeB broker verification");
  18  |   await page
  19  |     .getByRole("button", { name: "Create project", exact: true })
  20  |     .click();
  21  |   for (const label of [
  22  |     "Evolved Packet Core",
  23  |     "LTE eNodeB",
  24  |     "LTE eNodeB",
  25  |     "User Equipment",
  26  |   ])
  27  |     await page
  28  |       .locator(".library-items button")
  29  |       .filter({ hasText: label })
  30  |       .click();
  31  |   await expect(page.locator(".network-node")).toHaveCount(4);
  32  |   await expect(page.locator(".react-flow__edge")).toHaveCount(4);
  33  |   await page.getByRole("button", { name: "Validate", exact: true }).click();
  34  |   await expect(page.locator(".adapter-blockers")).toHaveCount(0);
  35  |   await expect(
  36  |     page.locator(".check-row").filter({ hasText: "Native execution adapter" }),
  37  |   ).toContainText("Supported");
  38  |   await page
  39  |     .getByRole("button", { name: "Start LTE network", exact: true })
  40  |     .click();
  41  |   try {
  42  |     await expect(
  43  |       page.getByText("NAS evidence verified", { exact: true }),
> 44  |     ).toBeVisible({ timeout: 110000 });
      |       ^ Error: expect(locator).toBeVisible() failed
  45  |     await expect(page.locator(".process-row")).toHaveCount(5);
  46  |     await expect(page.locator('[data-component-id="broker"]')).toContainText(
  47  |       /PID \d+/,
  48  |     );
  49  |     await expect(page.locator(".runtime-indicator")).toContainText(
  50  |       "Simulation running",
  51  |     );
  52  |     let native: any;
  53  |     await expect
  54  |       .poll(
  55  |         async () => {
  56  |           native = await (await page.request.get("/api/simulations")).json();
  57  |           return native.serving_cell?.pci;
  58  |         },
  59  |         { timeout: 15000 },
  60  |       )
  61  |       .toBe(1);
  62  |     const enbs = native.run.topology.nodes.filter(
  63  |       (n: { kind: string }) => n.kind === "enb",
  64  |     );
  65  |     expect(
  66  |       new Set(enbs.map((n: { id: string }) => native.node_components[n.id].pid))
  67  |         .size,
  68  |     ).toBe(2);
  69  |     await expect.poll(async () => {
  70  |       native = await (await page.request.get("/api/simulations")).json();
  71  |       return native.run.broker?.data.paths?.length === 2 && native.run.broker.data.paths.every((p: {input_samples:number;input_energy:number})=>p.input_samples>0 && p.input_energy>0);
  72  |     },{timeout:15000}).toBe(true);
  73  |     await page.screenshot({
  74  |       path: "../docs/multicell-native-runtime.png",
  75  |       fullPage: true,
  76  |     });
  77  |     await page
  78  |       .getByRole("button", { name: "ZeroMQ / RF Transport", exact: true })
  79  |       .click();
  80  |     await expect(
  81  |       page.getByRole("heading", {
  82  |         name: "Applied I/Q broker routing",
  83  |         exact: true,
  84  |       }),
  85  |     ).toBeVisible();
  86  |     await expect(
  87  |       page
  88  |         .locator(".surface")
  89  |         .filter({
  90  |           has: page.getByRole("heading", {
  91  |             name: "Applied I/Q broker routing",
  92  |           }),
  93  |         })
  94  |         .getByRole("row"),
  95  |     ).toHaveCount(3);
  96  |     expect(errors).toEqual([]);
  97  |   } finally {
  98  |     await page.request.post("/api/simulations/stop", {
  99  |       headers: { Origin: "http://127.0.0.1:3000" },
  100 |     });
  101 |   }
  102 | });
  103 | 
```