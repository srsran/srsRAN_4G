# Native multi-cell runtime

Studio now executes **one EPC, one UE and two or three co-channel LTE cells across one to three eNodeB processes**. This includes multiple owned cells in one eNodeB and the two-eNodeB layout previously blocked by the adapter. The original direct single-cell runner remains available.

The installed srsRAN executables and upstream examples are unchanged. Studio's additional C++ broker uses the installed libzmq and real FC32 I/Q. The launcher builds only this broker into the ignored private `var/tools/` directory. Rebuilding requires `g++` and the libzmq development headers; failure to build leaves single-cell mode available and reports a multi-cell prerequisite blocker.

## Run the saved network

1. Restart Studio with `./roguard-studio/scripts/start.sh` so the backend loads the new adapter.
2. Keep one EPC and UE. Configure two or three total cells, owned by one to three eNodeBs. Use unique eNodeB IDs, separate loopback S1/GTP bindings and unique PCIs. Co-channel cells owned by the same eNodeB must also have distinct PSS indices (`PCI % 3`). Shared cell settings must use 50 PRBs, the same Band 7 EARFCN, matching TAC/PLMN and info logging.
3. Save and Validate. Native execution adapter should show **Supported**. Unsupported shapes, duplicate process identities and missing host prerequisites remain blocked with specific diagnostics.
4. Start LTE network. Runtime lists each actual eNodeB and the I/Q broker separately. Running requires real NAS attachment and S1 setup from every eNodeB. The UE serving cell is resolved from attached native PCI/EARFCN and the applied configuration.
5. Review **ZeroMQ / RF Transport** for the explicit allocated RF map and native socket evidence. Native files previews show every process's `.conf`/`rr.conf` and model trace.

The first cell of a newly created design defaults to 0 dB broker attenuation; additional new paths default to 20 dB. These are editable radio-model inputs. Existing saved designs keep their values (legacy missing values read as 0 dB). The native two-eNodeB equal-gain case was verified. The equal-gain three-eNodeB test timed out during NAS attach while all paths carried I/Q; it is retained as a negative experiment, not presented as success. For the verified three-cell baseline, attenuate the other two candidates by 20 dB.

## I/Q model and isolation

Each native eNodeB TX and broker UL listener gets a distinct private-loopback endpoint. UE TX and broker DL use another pair. A fresh network namespace per run prevents collisions with other runs and host services. Cell RF indices within one eNodeB map to indexed `tx_portN`/`rx_portN` native options.

The broker continuously requests each native source, buffers arbitrary message sizes without discarding samples, and aligns paths from sample index zero. It sends the UE the sum of each path's actual complex samples multiplied by `10^(-attenuation_dB/20)`. It fans the UE uplink to all cell receivers with the same per-path attenuation. There is no hidden normalization, added noise, geographical path loss, propagation delay or mobility schedule. Bounded queues apply backpressure; source/consumer loss fails the broker or native supervisor. Gain edits require a full restart and do not mutate a running configuration.

Transport counters and input energy (the dimensionless sum of squared FC32 magnitudes, not joules) are measured by the compiled broker and labeled separately from native UE CSV measurements. Nonzero input energy confirms that a path contains actual samples, not that the UE decoded that cell. Selection tests separately verify actual serving PCI with each chosen path dominant. Native virtual-radio RSRP is not a calibrated field-strength measurement.

The RF map is derived and reviewed before startup. Multi-cell mode uses its allocated endpoints rather than the legacy direct point-to-point port fields. The applied run stores that map and configuration hashes; saving a design never rewrites it.

## Evidence and limits

- [Native probe cases and negative experiment](multicell-native-verification.json)
- [Two-eNodeB public API acceptance](multicell-2enb-2cell-acceptance.json)
- [Three separate eNodeBs / three-cell public API acceptance](multicell-3enb-3cell-acceptance.json)
- [Mixed two-eNodeB / three-cell acceptance](multicell-2enb-3cell-acceptance.json)
- [Broker crash, cleanup and successful restart](multicell-failure-acceptance.json)
- [Existing Roguard bridge replay of native multi-cell evidence](multicell-bridge-validation.json)
- [Three-eNodeB evidence replay into the existing bridge](multicell-three-enb-bridge-validation.json)
- [Preserved binaries, templates and broker audit](multicell-runtime-audit.json)
- [Native browser runtime screenshot](multicell-native-runtime.png)

Native logs, CSVs, `rf-plan.json` and generated configurations remain in the recorded private run directories. Protocol fixture tests additionally verify binary framing, variable-size alignment, the actual sum, reciprocal attenuation, UL fan-out, listener collision handling and graceful termination.

**Handover is not enabled or claimed.** Selecting a cell during a new run is not handover. Neighbor/report configuration, runtime gain schedules, mobility and completed-handover event ingestion still require separate integration. The bundled srsEPC does not implement S1 handover; see the [official application note](https://docs.srsran.com/projects/4g/en/latest/app_notes/source/handover/source/index.html). Different frequencies, more than three cells, multiple UEs/EPCs and other broker bandwidths remain outside this verified adapter.

Final regression scope: 45 backend/broker tests passed. The four browser cases passed individually: owned-cell editing/preview/unsupported-shape rejection, actual two-eNodeB attachment, existing single-cell lifecycle/export, and responsive/history/grouping controls. Actual API and probe checks are separate from protocol fixtures.

Final TypeScript validation and production build also passed. Original LTE executable hashes and all upstream example hashes match the preserved baseline audit. No commit or deployment was made.


## Duplicate PCI repair

The two-cell design with both PCIs set to 6 is invalid for the verified selection profile. Keep Cell 01 at PCI 6 and change Cell 02 to PCI 1; these use PSS indices 0 and 1. Cell IDs, RF-port indices and ownership stay unchanged. New cells now choose an unused PCI and sibling PSS index while preserving existing operator edits. Validation names the conflicting cells and offers a design-only correction, with stale-suggestion checks; configuration errors do not imply the native adapter is missing.

Broker validation uses the allocated RF map rather than requiring the old direct eNodeB/UE port fields to match. Removing an eNodeB also synchronizes the remaining one-eNodeB/one-UE design's fallback pair. Direct single-cell mode still requires reciprocal endpoints. Existing applied run files are never rewritten by these repairs.

The saved baseline laboratory was backed up privately and repaired through the revision-checked control API (revision 25): PCIs 6/1 and reciprocal fallback ports for eNodeB 412. Its running backend reports topology valid, prerequisites ready, adapter supported and runnable. Reload the browser to obtain the saved design before validating and starting.

PCI regression validation: 52 backend/broker tests passed, including minimal repairs, reservation of other valid cell identities, same-PSS detection, generator defense and broker/direct endpoint validation.


The PCI-repair browser regression also passed: create eNodeB 412, remove the original eNodeB, add a compatible second cell, manually reproduce PCI 6/6, apply the suggested PCI 1 repair, preserve both cell identities, and obtain real NAS attachment with both broker paths carrying samples. [Browser evidence](pci-repair-acceptance.json), [runtime screenshot](pci-repair-native-runtime.png) and [independent native API check with stale direct ports](pci-repair-api-acceptance.json) are retained. TypeScript validation and production build passed.
