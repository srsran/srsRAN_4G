"use client";
import { useEffect, useState } from "react";
import { useQuery } from "@tanstack/react-query";
import {
  Activity,
  ChevronDown,
  Code2,
  Info,
  Plus,
  Radio,
  Settings2,
  Trash2,
} from "lucide-react";
import {
  api,
  useStudio,
  labels,
  type Config,
  type CellConfig,
  type Status,
  type Validation,
  type RFPlan,
} from "@/lib/studio";
import { Button } from "./ui/button";

type NativeField = {
  description: string;
  native_field: string;
  file: string;
  min: number | null;
  max: number | null;
  default: unknown;
  dependencies: string;
  restart: string;
  support: string;
};
type Preview = {
  rf_plan?: RFPlan | null;
  diffs: Record<string, string>;
  applied_run_id: string | null;
  scope: string;
  files: Record<string, string>;
  validation: Validation;
  trace: unknown[];
};
const cellLabels: Record<keyof CellConfig, string> = {
  cell_id: "Cell ID",
  pci: "Physical cell ID",
  tac: "Tracking area code",
  dl_earfcn: "DL EARFCN",
  ul_earfcn: "UL EARFCN (optional)",
  rf_port: "RF port",
  root_seq_idx: "PRACH root sequence",
  ho_active: "Handover activation",
};
export function Inspector({
  status,
  validation,
  onRestart,
}: {
  status?: Status;
  validation?: Validation;
  onRestart: () => void;
}) {
  const { topology, selected, edit, setSelected, addCell } = useStudio();
  const owner = topology.nodes.find((n) =>
    n.cells?.some((c) => c.id === selected[0]),
  );
  const cell = owner?.cells?.find((c) => c.id === selected[0]);
  const node = owner ?? topology.nodes.find((n) => n.id === selected[0]);
  const [tab, setTab] = useState("configure");
  const capability = useQuery({
    queryKey: ["cell-field-metadata"],
    queryFn: () =>
      api<{ fields: Record<string, NativeField> }>("/capabilities"),
    retry: 2,
  });
  const fields = capability.data?.fields ?? {};
  const [preview, setPreview] = useState<Preview>();
  const [error, setError] = useState("");
  useEffect(() => {
    setPreview(undefined);
    setError("");
  }, [topology]);
  if (!node)
    return (
      <aside className="inspector">
        <div className="panel-heading">
          <Settings2 size={15} /> NETWORK INSPECTOR
        </div>
        <div className="inspector-empty">
          <Radio size={30} />
          <h3>Select a process or owned cell</h3>
          <p>Add EPC, eNodeB and UE processes. Add cells inside an eNodeB.</p>
        </div>
      </aside>
    );
  const appliedNode = status?.run?.topology.nodes.find((n) => n.id === node.id);
  const applied =
    !!appliedNode &&
    (!cell || appliedNode.cells?.some((c) => c.id === cell.id));
  const isProcess = ["epc", "enb", "ue"].includes(node.kind);
  const runtime = applied
    ? (status?.node_components?.[node.id] ?? status?.components[node.kind])
    : undefined;
  const setConfig = (key: keyof Config, value: unknown) =>
    edit((t) => {
      const n = t.nodes.find((n) => n.id === node.id)!;
      n.config = { ...n.config, [key]: value };
    });
  const input = (
    key: keyof Config,
    label: string,
    options?: (number | string)[],
  ) => {
    const value = node.config[key];
    return (
      <label className="field" key={key}>
        <span>{label}</span>
        {options ? (
          <select
            value={String(value)}
            onChange={(e) =>
              setConfig(
                key,
                typeof value === "number"
                  ? Number(e.target.value)
                  : e.target.value,
              )
            }
          >
            {options.map((o) => (
              <option key={o}>{o}</option>
            ))}
          </select>
        ) : typeof value === "boolean" ? (
          <input
            type="checkbox"
            checked={value}
            onChange={(e) => setConfig(key, e.target.checked)}
          />
        ) : (
          <input
            type={typeof value === "number" ? "number" : "text"}
            value={String(value)}
            onChange={(e) =>
              setConfig(
                key,
                typeof value === "number"
                  ? Number(e.target.value)
                  : e.target.value,
              )
            }
          />
        )}
      </label>
    );
  };
  const section = (title: string, content: React.ReactNode, open = false) => (
    <details open={open}>
      <summary>
        {title}
        <ChevronDown size={13} />
      </summary>
      {content}
    </details>
  );
  return (
    <aside className="inspector">
      <div className="panel-heading">
        <Settings2 size={15} />{" "}
        {cell ? "CELL CONFIGURATION" : "PROCESS INSPECTOR"}
      </div>
      <div className="inspector-identity">
        <div className="inspector-node-icon">
          <Radio size={25} />
        </div>
        <div>
          <h3>{cell?.name ?? node.name}</h3>
          <span>{cell ? `Owned by ${node.name}` : labels[node.kind]}</span>
        </div>
      </div>
      <div className="inspector-badges">
        <span className="badge">
          {applied ? (runtime?.state ?? "stopped") : "DESIGNED"}
        </span>
        <span className="badge cyan-text">
          {cell
            ? "OWNED CONFIGURATION"
            : isProcess
              ? "NATIVE PROCESS"
              : "DESIGN ANNOTATION"}
        </span>
      </div>
      <div className="tabs inspector-tab-buttons">
        {["configure", "native", "runtime"].map((v) => (
          <button key={v} onClick={() => setTab(v)} aria-pressed={tab === v}>
            {v === "native"
              ? "Native files"
              : v === "runtime"
                ? "Runtime"
                : "Configure"}
          </button>
        ))}
      </div>
      <div className="inspector-body">
        {tab === "configure" && (
          <>
            <div className="config-notice">
              <Info size={14} />
              <span>
                Design edits need validation and a full restart. Saving never
                changes the applied radio.
              </span>
            </div>
            <label className="field">
              <span>{cell ? "Cell name" : "Component name"}</span>
              <input
                value={cell?.name ?? node.name}
                onChange={(e) =>
                  edit((t) => {
                    const n = t.nodes.find((n) => n.id === node.id)!;
                    if (cell)
                      n.cells!.find((c) => c.id === cell.id)!.name =
                        e.target.value;
                    else n.name = e.target.value;
                  })
                }
              />
            </label>
            {cell ? (
              <>
                <Button onClick={() => setSelected([node.id])}>
                  ← {node.name}
                </Button>
                {section(
                  "Cell radio identity",
                  <>
                    {(Object.keys(cellLabels) as (keyof CellConfig)[]).map(
                      (key) => {
                        const meta = fields[key],
                          value = cell.config[key];
                        const blocked =
                          key === "ho_active" || key === "rf_port";
                        return (
                          <div className="native-field" key={key}>
                            <label className="field">
                              <span>{cellLabels[key]}</span>
                              {typeof value === "boolean" ? (
                                <input
                                  type="checkbox"
                                  checked={value}
                                  disabled={blocked}
                                  readOnly
                                />
                              ) : (
                                <input
                                  type="number"
                                  value={value ?? ""}
                                  min={meta?.min ?? undefined}
                                  max={meta?.max ?? undefined}
                                  disabled={blocked}
                                  placeholder={
                                    key === "ul_earfcn"
                                      ? "Derived from DL"
                                      : undefined
                                  }
                                  onChange={(e) =>
                                    edit((t) => {
                                      const c = t.nodes
                                        .find((n) => n.id === node.id)!
                                        .cells!.find((c) => c.id === cell.id)!;
                                      c.config = {
                                        ...c.config,
                                        [key]:
                                          e.target.value === "" &&
                                          key === "ul_earfcn"
                                            ? null
                                            : Number(e.target.value),
                                      };
                                    })
                                  }
                                />
                              )}
                            </label>
                            <code>rr.conf → cell_list[].{key}</code>
                            <p className="help-text">
                              {meta?.description}
                              <br />
                              Default: {String(
                                meta?.default ?? "derived",
                              )} ·{" "}
                              {meta?.min != null
                                ? `${meta.min}–${meta.max} · `
                                : ""}
                              Restart: full chain
                              <br />
                              {meta?.dependencies}
                            </p>
                          </div>
                        );
                      },
                    )}
                  </>,
                  true,
                )}
                {section(
                  "Measurements, handover & SIB",
                  <p className="help-text">
                    The native parser supports meas_cell_list and
                    meas_report_desc (A1–A5). These are configuration entries,
                    not running neighbors. Brokered attachment is integrated.
                    Handover still requires neighbor/report configuration and
                    native completion-event verification. Shared SIB1/2/3 values
                    remain in the audited upstream sib.conf; arbitrary edits are
                    not applied.
                  </p>,
                )}
              </>
            ) : isProcess ? (
              <>
                {section(
                  "Identity",
                  <>
                    <div className="field-grid">
                      {input("mcc", "MCC", ["001"])}
                      {input("mnc", "MNC")}
                      {node.kind === "enb" && input("enb_id", "eNodeB ID")}
                      {node.kind === "epc" &&
                        input("tac", "Core tracking area code")}
                    </div>
                    {node.kind === "ue" &&
                      input("imsi", "Synthetic laboratory IMSI")}
                  </>,
                  true,
                )}
                {node.kind === "enb" &&
                  section(
                    "Cells",
                    <>
                      <p className="help-text">
                        {node.cells?.length ?? 0} owned cells. Current native
                        execution supports up to 3 total co-channel cells. Use
                        native preview to review allocated broker endpoints.
                      </p>
                      {node.cells?.map((c) => (
                        <button
                          className="owned-cell inspector-cell"
                          key={c.id}
                          onClick={() => setSelected([c.id])}
                        >
                          <Radio size={15} />
                          <span>
                            {c.name}
                            <small>
                              PCI {c.config.pci} · EARFCN {c.config.dl_earfcn} ·
                              RF {c.config.rf_port}
                            </small>
                          </span>
                        </button>
                      ))}
                      <Button
                        onClick={() => addCell(node.id)}
                        disabled={(node.cells?.length ?? 0) >= 5}
                      >
                        <Plus size={14} /> Add Cell
                      </Button>
                    </>,
                    true,
                  )}
                {(node.kind === "enb" || node.kind === "ue") &&
                  section(
                    "Radio interfaces / ZeroMQ",
                    <>
                      <p className="help-text">
                        {node.kind === "enb"
                          ? "enb.conf [enb] n_prb owns bandwidth; cell identity is configured in Cells."
                          : "ue.conf [rat.eutra] dl_earfcn configures cell search, not a serving-cell assignment."}
                      </p>
                      <div className="field-grid">
                        {node.kind === "enb"
                          ? input(
                              "n_prb",
                              "Bandwidth · PRB",
                              [6, 15, 25, 50, 100],
                            )
                          : input("earfcn", "Search DL EARFCN")}
                        {input("tx_gain", "TX gain · dB")}
                        {input("rx_gain", "RX gain · dB")}
                        {input("tx_port", "TX loopback port")}
                        {input("rx_port", "RX loopback port")}
                      </div>
                      {input("device_id", "Device identifier")}
                      <code className="device-args">
                        device_name=zmq
                        <br />
                        tx_port=tcp://127.0.0.1:{node.config.tx_port}
                        <br />
                        rx_port=tcp://127.0.0.1:{node.config.rx_port}
                        <br />
                        base_srate=23040000,fail_on_disconnect=true
                      </code>
                      <p className="help-text">
                        Native I/Q transport uses reciprocal endpoints in the
                        single-cell adapter. It is not evidence of attachment.
                        Multi-cell mode uses the I/Q broker and allocated
                        endpoints shown in native preview; the point-to-point
                        ports above apply to single-cell mode.
                      </p>
                    </>,
                  )}
                {section(
                  "Core network",
                  <>
                    {(node.kind === "epc" || node.kind === "enb") &&
                      input("mme_addr", "MME loopback address")}
                    {node.kind === "enb" &&
                      input("bind_addr", "S1 / GTP loopback binding")}
                    {(node.kind === "epc" || node.kind === "ue") &&
                      input("apn", "APN")}
                    <p className="help-text">
                      S1 uses native core addresses. Design edges do not force
                      an attachment.
                    </p>
                  </>,
                )}
                {section(
                  "Native logging",
                  <>
                    {input("log_level", "Log level", [
                      "info",
                      "warning",
                      "error",
                    ])}
                    {node.kind !== "epc" &&
                      input("metrics_enabled", "CSV metrics")}
                    <p className="help-text">
                      Info logging is required for readiness evidence. Output
                      paths and generated subscriber credentials stay private.
                    </p>
                  </>,
                )}
                {node.kind === "enb" &&
                  section(
                    "Native downlink impairment",
                    <>
                      {input("awgn_enabled", "Enable AWGN")}
                      {input("awgn_snr", "AWGN SNR · dB")}
                      <p className="help-text">
                        enb.conf [channel.dl] and [channel.dl.awgn]. Restart
                        required. This executable noise model does not use
                        canvas distance or imply motion.
                      </p>
                    </>,
                  )}
              </>
            ) : (
              <p className="help-text">
                Legacy design annotation. This object never launches a native
                process.
              </p>
            )}
            {validation && (
              <p className="help-text">
                Last saved validation:{" "}
                {validation.valid ? "valid design" : "configuration errors"}.{" "}
                {validation.runnable
                  ? "Execution prerequisites passed"
                  : !validation.adapter_supported
                    ? "Native execution adapter does not support this design"
                    : "Runtime prerequisites need attention"}
                .
              </p>
            )}
          </>
        )}
        {tab === "configure" && cell && (
          <label className="field">
            <span>Broker path attenuation · dB</span>
            <input
              type="number"
              min="0"
              max="80"
              value={cell.rf_attenuation_db ?? 0}
              onChange={(e) =>
                edit((t) => {
                  t.nodes
                    .find((n) => n.id === node.id)!
                    .cells!.find((c) => c.id === cell.id)!.rf_attenuation_db =
                    Number(e.target.value);
                })
              }
            />
            <small>
              Actual DL/UL I/Q amplitude scaling in broker mode. 20 dB gives 0.1
              amplitude. Requires restart; use 0 dB for direct single-cell mode.
              No geographic propagation model.
            </small>
          </label>
        )}
        {tab === "native" && (
          <>
            <div className="config-notice">
              <Code2 size={15} />
              <span>
                Preview the current unsaved design. Temporary subscriber
                credentials are redacted; applied files are never changed.
              </span>
            </div>
            <Button
              onClick={() => {
                setError("");
                void api<Preview>("/configurations/preview", "POST", topology)
                  .then((result) => {
                    if (useStudio.getState().topology === topology)
                      setPreview(result);
                  })
                  .catch((e) => {
                    if (useStudio.getState().topology === topology)
                      setError(String(e));
                  });
              }}
            >
              Generate native preview
            </Button>
            {error && <p role="alert">{error}</p>}
            {preview && (
              <>
                <p className="help-text">
                  {preview.scope} ·{" "}
                  {preview.validation.valid &&
                  preview.validation.adapter_supported
                    ? preview.rf_plan
                      ? "Native co-channel broker generation"
                      : "Single-cell generation"
                    : "Design-only generation; execution blocked"}
                </p>
                {[
                  ...preview.validation.errors,
                  ...(preview.validation.adapter_blockers ?? []),
                ].map((e) => (
                  <p className="help-text" key={e}>
                    {e}
                  </p>
                ))}
                {Object.entries(preview.files).map(([name, text]) => (
                  <details key={name}>
                    <summary>
                      {name}
                      <ChevronDown size={13} />
                    </summary>
                    <pre className="code-block">{text}</pre>
                  </details>
                ))}
                <details>
                  <summary>
                    Changes against{" "}
                    {preview.applied_run_id ?? "empty applied state"}
                  </summary>
                  {Object.entries(preview.diffs).map(([name, diff]) => (
                    <pre className="code-block" key={name}>
                      {diff || `${name}: no change`}
                    </pre>
                  ))}
                </details>
                {preview.rf_plan && (
                  <details>
                    <summary>Allocated I/Q broker routing</summary>
                    <pre className="code-block">
                      {JSON.stringify(preview.rf_plan, null, 2)}
                    </pre>
                  </details>
                )}
                <details>
                  <summary>Model → native trace</summary>
                  <pre className="code-block">
                    {JSON.stringify(preview.trace, null, 2)}
                  </pre>
                </details>
              </>
            )}
          </>
        )}
        {tab === "runtime" && (
          <>
            <div className="info-card">
              <Activity size={18} />
              <h3>
                {applied ? (runtime?.state ?? "stopped") : "No applied runtime"}
              </h3>
              <p>
                {cell
                  ? "Cells share their owning eNodeB process; they do not have separate PIDs."
                  : isProcess
                    ? "State and PID come from the owned process supervisor."
                    : "Design annotation; no executable process."}
              </p>
            </div>
            <div className="keyvalue">
              <span>Process PID</span>
              <code>{runtime?.pid ?? "Not available"}</code>
            </div>
            {node.kind === "ue" && (
              <>
                <div className="keyvalue">
                  <span>Attachment</span>
                  <code>
                    {applied && status?.attached
                      ? "Native evidence"
                      : "Not evidenced"}
                  </code>
                </div>
                <div className="keyvalue">
                  <span>Serving cell</span>
                  <code>
                    {applied
                      ? (status?.serving_cell?.name ?? "Unresolved")
                      : "Not observed"}
                  </code>
                </div>
                {applied && status?.serving_cell && (
                  <p className="help-text">
                    PCI {status.serving_cell.pci} · EARFCN{" "}
                    {status.serving_cell.earfcn}
                    <br />
                    {status.serving_cell.provenance}
                  </p>
                )}
              </>
            )}
            <Button disabled={!applied || !isProcess} onClick={onRestart}>
              Open runtime / restart chain
            </Button>
          </>
        )}
      </div>
      <div className="inspector-footer">
        <span>
          {cell
            ? "Cell configuration"
            : isProcess
              ? "Native process"
              : "Design annotation"}{" "}
          · neutral
        </span>
        <Button
          size="icon"
          title={cell ? "Delete cell" : "Delete component"}
          onClick={() => useStudio.getState().remove()}
        >
          <Trash2 size={14} />
        </Button>
      </div>
    </aside>
  );
}
