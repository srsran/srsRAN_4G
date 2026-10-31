"use client";
import { CapabilityMatrix, Environment } from "./environment";
import { useState } from "react";
import { useQuery, useQueryClient } from "@tanstack/react-query";
import {
  Activity,
  AlertTriangle,
  ArrowRight,
  Check,
  CheckCircle2,
  ChevronRight,
  Cable,
  Download,
  FileJson,
  FlaskConical,
  FolderOpen,
  Info,
  Play,
  Plus,
  RadioTower,
  RefreshCw,
  Server,
  Shield,
  ShieldCheck,
  Square,
  Terminal,
  XCircle,
} from "lucide-react";
import {
  api,
  downloadJSON,
  labels,
  useStudio,
  type Kind,
  type Metric,
  type RFPlan,
  type Status,
  type System,
  type Validation,
} from "@/lib/studio";
import { nodeIcons } from "./topology";
import { Button } from "./ui/button";
import { Telemetry } from "./telemetry";
type Shared = {
  busy: boolean;
  status?: Status;
  system?: System;
  validation?: Validation;
  metrics: Metric[];
  act: (path: string, body?: unknown) => Promise<unknown>;
  save: () => Promise<unknown>;
  validate: () => void;
  start: () => void;
  stop: () => void;
  restart: () => void;
  notify: (s: string) => void;
  go: (s: string) => void;
  capture: () => void;
};
type Event = {
  id: string;
  timestamp: string;
  type: string;
  message: string;
  [key: string]: unknown;
};
type Scenario = {
  id: string;
  name: string;
  category: string;
  description: string;
  method: string;
  expected: string;
};
type Experiment = {
  id: string;
  name: string;
  timestamp: string;
  category: string;
  ground_truth: string;
  topology: { name: string };
  metrics?: Metric[];
};
function PageTitle({
  eyebrow,
  title,
  description,
  children,
}: {
  eyebrow: string;
  title: string;
  description: string;
  children?: React.ReactNode;
}) {
  return (
    <div className="page-title">
      <div>
        <span className="eyebrow">{eyebrow}</span>
        <h1>{title}</h1>
        <p>{description}</p>
      </div>
      {children}
    </div>
  );
}
export function WorkspacePage({ page, ...p }: Shared & { page: string }) {
  const { topology, edit, setSelected } = useStudio();
  const queryClient = useQueryClient();
  const [target, setTarget] = useState("");
  const [radioTarget, setRadioTarget] = useState("");
  const [params, setParams] = useState("{}");
  const [reason, setReason] = useState("");
  const [format, setFormat] = useState("zip");
  const scenarios = useQuery({
    queryKey: ["scenarios"],
    queryFn: () => api<Scenario[]>("/scenarios"),
    enabled: page === "Scenario Laboratory",
  });
  const scenarioEvents = useQuery({
    queryKey: ["scenario-events"],
    queryFn: () =>
      api<
        {
          id: string;
          topology_id: string;
          scenario_id: string;
          target: string;
          timestamp: string;
          state: string;
          ground_truth: string;
        }[]
      >("/scenarios/events"),
    enabled: page === "Scenario Laboratory",
    refetchInterval: 4000,
  });
  const experiments = useQuery({
    queryKey: ["experiments"],
    queryFn: () => api<Experiment[]>("/experiments"),
    enabled: page === "Experiment Manager",
    refetchInterval: 5000,
  });
  const events = useQuery({
    queryKey: ["events"],
    queryFn: () => api<Event[]>("/events"),
    enabled: ["Logs & Diagnostics", "Capability Matrix"].includes(page),
    refetchInterval: 3000,
  });
  const zmq = useQuery({
    queryKey: ["zeromq"],
    queryFn: () =>
      api<{
        host: {
          available: boolean;
          scope?: string;
          reason?: string;
          sockets: {
            pid: number;
            local: string;
            remote: string;
            status: string;
          }[];
        };
        runtime?: { text: string; source: string };
        configuration_changes: string;
      }>("/zeromq"),
    enabled: page === "ZeroMQ / RF Transport",
    refetchInterval: 3000,
  });
  const plan = useQuery({
    queryKey: ["plan", topology.id],
    queryFn: () =>
      api<{
        rf_plan?: RFPlan | null;
        steps: string[];
        affected: { id: string; name: string; kind: string }[];
        note: string;
      }>(`/topologies/${topology.id}/plan`),
    enabled: ["Simulation Runtime", "ZeroMQ / RF Transport"].includes(page),
    retry: false,
  });
  const changes = useQuery({
    queryKey: ["topology-events"],
    queryFn: () =>
      api<
        {
          id: string;
          topology_id: string;
          timestamp: string;
          classification: string;
          observed: { parameters: Record<string, number>; source: string };
          analyst_decision: string | null;
        }[]
      >("/topology-events"),
    enabled: page === "Capability Matrix",
    refetchInterval: 4000,
  });
  const baselines = useQuery({
    queryKey: ["baselines"],
    queryFn: () =>
      api<
        {
          topology_id: string;
          parameters: Record<string, number>;
          source: string;
        }[]
      >("/baselines"),
    enabled: page === "Capability Matrix",
    refetchInterval: 4000,
  });
  const savedTopologies = useQuery({
    queryKey: ["topologies"],
    queryFn: () =>
      api<{ id: string; name: string; nodes: unknown[]; revision: number }[]>(
        "/topologies",
      ),
    enabled: page === "Configuration Files",
  });
  const enbs = topology.nodes.filter((n) => n.kind === "enb");
  const enb = enbs.find((n) => n.id === radioTarget) ?? enbs[0],
    ue = topology.nodes.find((n) => n.kind === "ue");
  const multiRadio = topology.nodes.flatMap((n) => n.cells ?? []).length > 1;
  const appliedPlan =
    p.status?.run?.topology.id === topology.id ? p.status.run.rf_plan : null;
  const routePlan = appliedPlan ?? plan.data?.rf_plan;
  const processRows: {
    key: string;
    label: string;
    kind: string;
    state: Status["components"][string] | undefined;
  }[] = (p.status?.run?.topology ?? topology).nodes
    .filter((n) => ["epc", "enb", "ue"].includes(n.kind))
    .map((n) => ({
      key: n.id,
      label: n.name,
      kind: n.kind,
      state: p.status?.node_components?.[n.id] ?? p.status?.components[n.kind],
    }));
  if (p.status?.run?.rf_plan)
    processRows.push({
      key: "broker",
      label: "FC32 I/Q broker",
      kind: "broker",
      state: p.status.components.broker,
    });
  const mutation = (path: string, body?: unknown) =>
    p.act(path, body).then(() => queryClient.invalidateQueries());
  if (page === "Capability Matrix") return <CapabilityMatrix />;
  if (page === "Radio Environment" || page === "Mobility Experiments")
    return (
      <Environment
        status={p.status}
        mobility={page === "Mobility Experiments"}
      />
    );
  if (page === "Native Measurements") return <Telemetry metrics={p.metrics} />;
  if (page === "Component Inventory")
    return (
      <div className="page-content">
        <PageTitle
          eyebrow="TOPOLOGY INVENTORY"
          title="Network components"
          description="Designed components and actual process state, shown independently."
        />
        <div className="inventory-grid">
          {topology.nodes.map((n) => {
            const Icon = nodeIcons[n.kind];
            const applied = p.status?.run?.topology.nodes.some(
              (a) => a.id === n.id,
            );
            return (
              <button
                className="inventory-card"
                key={n.id}
                onClick={() => {
                  setSelected([n.id]);
                  p.go("Network Builder");
                }}
              >
                <Icon size={26} />
                <h3>{n.name}</h3>
                <p>{labels[n.kind]}</p>
                {n.cells?.map((c) => (
                  <span className="owned-cell" key={c.id}>
                    {c.name} · PCI {c.config.pci} · {c.config.dl_earfcn}{" "}
                    (configured)
                  </span>
                ))}
                <span className="badge">
                  {applied
                    ? ((
                        p.status?.node_components?.[n.id] ??
                        p.status?.components[
                          ["suspicious", "temporary"].includes(n.kind)
                            ? "enb"
                            : n.kind
                        ]
                      )?.state ?? "stopped")
                    : "DESIGNED · NOT APPLIED"}
                </span>
                <code>{n.id}</code>
                <span>
                  Inspect component <ChevronRight size={13} />
                </span>
              </button>
            );
          })}
        </div>
        {!topology.nodes.length && (
          <div className="empty-state">
            <Server size={30} />
            <h3>Your laboratory is empty</h3>
            <Button onClick={() => p.go("Network Builder")}>
              Open the topology editor
            </Button>
          </div>
        )}
      </div>
    );
  if (page === "Simulation Runtime")
    return (
      <div className="page-content">
        <PageTitle
          eyebrow="PROCESS ORCHESTRATION"
          title="Simulation control"
          description="Verified readiness, owned process groups and explicit restart plans."
        >
          <span
            className={`badge ${p.status?.state === "running" ? "success" : ""}`}
          >
            {p.status?.state ?? "ENGINE UNAVAILABLE"}
          </span>
        </PageTitle>
        <div className="control-actions">
          <Button disabled={p.busy || !p.system} onClick={p.validate}>
            <ShieldCheck size={14} /> Validate topology
          </Button>
          <Button
            variant="default"
            disabled={
              p.busy ||
              !p.system ||
              ["starting", "running", "stopping"].includes(
                p.status?.state ?? "",
              )
            }
            onClick={p.start}
          >
            <Play size={14} /> Start LTE network
          </Button>
          <Button disabled={p.busy || !p.status?.run} onClick={p.restart}>
            <RefreshCw size={14} /> Restart chain
          </Button>
          <Button
            disabled={
              p.busy ||
              !["starting", "running", "stopping"].includes(
                p.status?.state ?? "",
              )
            }
            onClick={p.stop}
          >
            <Square size={12} /> Stop network
          </Button>
        </div>
        <div className="two-col">
          <section className="surface">
            <h3>Configuration & environment validation</h3>
            {p.validation ? (
              <>
                <CheckRow
                  ok={p.validation.valid}
                  text="Topology configuration"
                />
                <CheckRow
                  ok={
                    p.validation.environment_ready ??
                    p.validation.runtime_blockers.length === 0
                  }
                  text="Runtime prerequisites"
                />
                <CheckRow
                  ok={p.validation.adapter_supported ?? true}
                  text="Native execution adapter"
                  passText="Supported"
                  failText="Unsupported configuration"
                />
                {(p.validation.adapter_blockers ?? []).length > 0 && (
                  <div className="adapter-blockers" role="note">
                    <h4>Native execution blocked</h4>
                    {p.validation.adapter_blockers.map((message) => (
                      <p className="help-text" key={message}>
                        {message}
                      </p>
                    ))}
                    <Button
                      onClick={() => {
                        if (enb) setSelected([enb.id]);
                        p.go("Network Builder");
                      }}
                    >
                      Review owned cells
                    </Button>
                  </div>
                )}
                {[...p.validation.errors, ...p.validation.runtime_blockers].map(
                  (s, i) => (
                    <div className="validation-error" key={i}>
                      <XCircle size={14} />
                      <span>{s}</span>
                    </div>
                  ),
                )}
                {(p.validation.suggested_pci_changes ?? []).length > 0 && (
                  <section className="config-notice">
                    <div>
                      <p>Suggested design-only PCI corrections:</p>
                      {p.validation.suggested_pci_changes!.map((fix) => (
                        <p key={fix.cell_id}>
                          {fix.cell_name}: PCI {fix.from_pci} → {fix.to_pci}
                        </p>
                      ))}
                      <Button
                        onClick={() => {
                          const fixes = p.validation!.suggested_pci_changes!;
                          const current = useStudio.getState().topology;
                          if (
                            fixes.some(
                              (fix) =>
                                current.nodes
                                  .find((n) => n.id === fix.enb_id)
                                  ?.cells?.find((c) => c.id === fix.cell_id)
                                  ?.config.pci !== fix.from_pci,
                            )
                          ) {
                            p.notify(
                              "Design changed since validation. Validate again before applying suggestions.",
                            );
                            return;
                          }
                          const proposed = structuredClone(current);
                          for (const fix of fixes)
                            proposed.nodes
                              .find((n) => n.id === fix.enb_id)!
                              .cells!.find(
                                (c) => c.id === fix.cell_id,
                              )!.config.pci = fix.to_pci;
                          if (
                            fixes.some((fix) => {
                              const owner = proposed.nodes.find(
                                (n) => n.id === fix.enb_id,
                              )!;
                              const cell = owner.cells!.find(
                                (c) => c.id === fix.cell_id,
                              )!;
                              return proposed.nodes.some((node) =>
                                (node.cells ?? []).some(
                                  (other) =>
                                    other.id !== cell.id &&
                                    other.config.dl_earfcn ===
                                      cell.config.dl_earfcn &&
                                    (other.config.pci === cell.config.pci ||
                                      (node.id === owner.id &&
                                        other.config.pci % 3 ===
                                          cell.config.pci % 3)),
                                ),
                              );
                            })
                          ) {
                            p.notify(
                              "Cell identities changed since validation. Validate again before applying suggestions.",
                            );
                            return;
                          }
                          edit((t) => {
                            for (const fix of fixes)
                              t.nodes
                                .find((n) => n.id === fix.enb_id)!
                                .cells!.find(
                                  (c) => c.id === fix.cell_id,
                                )!.config.pci = fix.to_pci;
                          });
                          p.notify(
                            "PCI corrections applied to the design. Save and validate to review; applied runtime is unchanged.",
                          );
                          p.go("Network Builder");
                          setSelected([fixes[0].cell_id]);
                        }}
                      >
                        Apply suggested PCIs to design
                      </Button>
                    </div>
                  </section>
                )}
                {p.validation.warnings.map((s, i) => (
                  <div className="warning-line" key={i}>
                    <AlertTriangle size={14} />
                    {s}
                  </div>
                ))}
              </>
            ) : (
              <p className="help-text">
                Save and validate the current design before running.
              </p>
            )}
          </section>
          <section className="surface">
            <h3>Applied process state</h3>
            {processRows.map((item) => (
              <div
                className="process-row"
                key={item.key}
                data-component-id={item.key}
              >
                <span className="process-icon">
                  {item.kind === "enb" ? (
                    <RadioTower size={18} />
                  ) : item.kind === "epc" ? (
                    <Server size={18} />
                  ) : (
                    <Activity size={18} />
                  )}
                </span>
                <div>
                  <strong>{item.label}</strong>
                  <p>PID {item.state?.pid ?? "—"}</p>
                </div>
                <span className="badge">
                  {item.state?.state ?? "NOT STARTED"}
                </span>
              </div>
            ))}
            {p.status?.run?.broker?.data.downlink_samples != null && (
              <p className="help-text">
                Broker DL samples: {p.status.run.broker.data.downlink_samples} ·
                UL samples: {p.status.run.broker.data.uplink_samples}. These are
                actual I/Q transport counters.
              </p>
            )}
            <div className="keyvalue">
              <span>UE attachment</span>
              <span className={p.status?.attached ? "cyan-text" : ""}>
                {p.status?.attached ? "NAS evidence verified" : "Not evidenced"}
              </span>
            </div>
            {p.status?.run?.error && (
              <pre className="error-text">{p.status.run.error}</pre>
            )}
          </section>
        </div>
        <section className="surface">
          <h3>
            Dependency restart plan{" "}
            <span className="badge amber-text">RESTART REQUIRED</span>
          </h3>
          <p className="help-text">
            {plan.data?.note ??
              "Save this topology to inspect its execution plan."}
          </p>
          <ol className="execution-plan">
            {plan.data?.steps.map((s, i) => (
              <li key={s}>
                <span>{String(i + 1).padStart(2, "0")}</span>
                {s}
              </li>
            ))}
          </ol>
          <div className="info-strip">
            Radio changes are saved as design changes. The applied snapshot
            updates only when a new real run begins. Pause/resume are
            unsupported.
          </div>
        </section>
      </div>
    );
  if (page === "ZeroMQ / RF Transport")
    return (
      <div className="page-content">
        <PageTitle
          eyebrow="VIRTUAL RADIO TRANSPORT"
          title="ZeroMQ manager"
          description="Designed native endpoints; runtime socket diagnostics are shown separately."
        >
          <span className={`badge ${p.system?.plugin ? "success" : ""}`}>
            {p.system?.plugin ? "PLUGIN DETECTED" : "PLUGIN UNAVAILABLE"}
          </span>
        </PageTitle>
        <div className="info-strip">
          <Info size={14} />
          {zmq.data?.configuration_changes ??
            "Configuration saved — restart required."}{" "}
          RF endpoints are restricted to isolated loopback TCP.
        </div>
        <label className="field">
          <span>eNodeB RF interface</span>
          <select
            value={enb?.id ?? ""}
            onChange={(e) => setRadioTarget(e.target.value)}
          >
            {enbs.map((n) => (
              <option key={n.id} value={n.id}>
                {n.name}
              </option>
            ))}
          </select>
        </label>
        {multiRadio && (
          <section className="surface">
            <h3>
              {appliedPlan ? "Applied" : "Saved design"} I/Q broker routing
            </h3>
            <p className="help-text">
              Two or three co-channel cells use real DL summation and UL
              fan-out. Direct point-to-point port fields are replaced by the
              explicit allocated routing below. Save/validate to refresh the
              saved plan; restart to apply edits.
            </p>
            {routePlan ? (
              <>
                <p className="help-text">
                  {routePlan.model} · {routePlan.sample_rate} samples/s · UE TX{" "}
                  {routePlan.ue_tx} / broker DL {routePlan.broker_dl}
                </p>
                <table>
                  <thead>
                    <tr>
                      <th>Cell</th>
                      <th>Native TX</th>
                      <th>Broker UL</th>
                      <th>RF port</th>
                      <th>Attenuation</th>
                    </tr>
                  </thead>
                  <tbody>
                    {routePlan.paths.map((path) => (
                      <tr key={path.cell_id}>
                        <td>{path.cell_id}</td>
                        <td>{path.native_tx}</td>
                        <td>{path.broker_ul}</td>
                        <td>{path.rf_port}</td>
                        <td>{path.attenuation_db} dB</td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </>
            ) : (
              <p className="help-text">
                Save and validate the design to inspect the allocated RF map.
              </p>
            )}
          </section>
        )}
        {!multiRadio && (
          <>
            <div className="transport-map">
              <div>
                <RadioTower size={30} />
                <h3>{enb?.name ?? "No eNodeB"}</h3>
                <span>eNodeB · zmq</span>
              </div>
              <div className="transport-directions">
                <div>
                  <code>TX :{enb?.config.tx_port ?? "—"}</code>
                  <span>
                    DOWNLINK I/Q <ArrowRight size={15} />
                  </span>
                  <code>RX :{ue?.config.rx_port ?? "—"}</code>
                </div>
                <div>
                  <code>RX :{enb?.config.rx_port ?? "—"}</code>
                  <span>
                    <ArrowRight size={15} className="reverse" /> UPLINK I/Q
                  </span>
                  <code>TX :{ue?.config.tx_port ?? "—"}</code>
                </div>
              </div>
              <div>
                <Activity size={30} />
                <h3>{ue?.name ?? "No UE"}</h3>
                <span>UE · zmq</span>
              </div>
            </div>
            <div className="two-col">
              {[enb, ue].map((n, i) => (
                <section className="surface" key={i}>
                  <h3>{i === 0 ? "eNodeB" : "UE"} transport configuration</h3>
                  {n ? (
                    <>
                      <div className="field-grid">
                        {(["tx_port", "rx_port"] as const).map((k) => (
                          <label className="field" key={k}>
                            <span>
                              {k === "tx_port"
                                ? "TX endpoint port"
                                : "RX endpoint port"}
                            </span>
                            <input
                              type="number"
                              value={n.config[k]}
                              onChange={(e) =>
                                edit((t) => {
                                  t.nodes.find((v) => v.id === n.id)!.config[
                                    k
                                  ] = Number(e.target.value);
                                })
                              }
                            />
                          </label>
                        ))}
                      </div>
                      <div className="keyvalue">
                        <span>Base sample rate</span>
                        <code>23.04 Msps</code>
                      </div>
                      <div className="keyvalue">
                        <span>Device ID</span>
                        <code>{n.config.device_id}</code>
                      </div>
                      <div className="keyvalue">
                        <span>Bind scope</span>
                        <code>127.0.0.1</code>
                      </div>
                    </>
                  ) : (
                    <p>Add this component in Topology Studio.</p>
                  )}
                </section>
              ))}
            </div>
          </>
        )}
        <div className="control-actions">
          <Button
            onClick={() =>
              p
                .save()
                .then(() => p.notify("Configuration saved — restart required."))
                .catch(() => {})
            }
          >
            Save transport settings
          </Button>
          <Button disabled={p.busy || !p.system} onClick={p.validate}>
            <ShieldCheck size={14} /> Detect endpoint conflicts
          </Button>
        </div>
        <section className="surface">
          <h3>Actual socket observations</h3>
          <p className="help-text">
            Host sockets and isolated runtime sockets are separate observations.
            Running RF sockets are captured inside the owned namespace.
          </p>
          {zmq.data?.runtime ? (
            <pre className="code-block">{zmq.data.runtime.text}</pre>
          ) : (
            <p className="help-text">
              No isolated runtime socket evidence has been collected.
            </p>
          )}
          {zmq.data?.host.available ? (
            <div className="table-wrap">
              <table>
                <thead>
                  <tr>
                    <th>Host local address</th>
                    <th>Remote</th>
                    <th>PID</th>
                    <th>State</th>
                  </tr>
                </thead>
                <tbody>
                  {zmq.data.host.sockets
                    .filter(
                      (s) =>
                        s.local?.endsWith(":" + enb?.config.tx_port) ||
                        s.local?.endsWith(":" + ue?.config.tx_port),
                    )
                    .map((s, i) => (
                      <tr key={i}>
                        <td>{s.local}</td>
                        <td>{s.remote ?? "—"}</td>
                        <td>{s.pid ?? "Unavailable"}</td>
                        <td>{s.status}</td>
                      </tr>
                    ))}
                </tbody>
              </table>
            </div>
          ) : (
            <p className="help-text">
              Host socket inspection unavailable: {zmq.data?.host.reason}
            </p>
          )}
          <code className="path-line">
            {p.system?.plugin ?? "Plugin not found"}
          </code>
        </section>
      </div>
    );
  if (page === "Scenario Laboratory")
    return (
      <div className="page-content">
        <PageTitle
          eyebrow="CONTROLLED RESEARCH ENVIRONMENT"
          title="Scenario laboratory"
          description="Ground truth labels belong to the experiment. They are distinct from predictions and analyst decisions."
        />
        <div className="scenario-setup">
          <label className="field">
            <span>Target component</span>
            <select value={target} onChange={(e) => setTarget(e.target.value)}>
              <option value="">Select a target</option>
              {topology.nodes.map((n) => (
                <option value={n.id} key={n.id}>
                  {n.name}
                </option>
              ))}
            </select>
          </label>
          <label className="field">
            <span>Synthetic event parameters · JSON object</span>
            <input value={params} onChange={(e) => setParams(e.target.value)} />
          </label>
        </div>
        <div className="scenario-grid">
          {scenarios.data?.map((s) => (
            <article
              className={`scenario-card ${s.category === "Suspicious" ? "suspicious" : ""}`}
              key={s.id}
            >
              <div className="scenario-card-top">
                <FlaskConical size={19} />
                <span className="badge">{s.category}</span>
              </div>
              <h3>{s.name}</h3>
              <p>{s.description}</p>
              <small>{s.expected}</small>
              <div className="scenario-method">
                <span className="badge amber-text">{s.method}</span>
                <Button
                  size="sm"
                  disabled={!target}
                  onClick={async () => {
                    try {
                      await p.save();
                      await mutation("/scenarios", {
                        scenario_id: s.id,
                        target,
                        topology_id: topology.id,
                        parameters: JSON.parse(params),
                      });
                      p.notify(
                        "Synthetic scenario recorded; live radio configuration is unchanged.",
                      );
                    } catch (e) {
                      p.notify(String(e));
                    }
                  }}
                >
                  <Play size={12} /> Record event
                </Button>
              </div>
            </article>
          ))}
        </div>
        <section className="surface">
          <h3>Recorded scenario events</h3>
          {scenarioEvents.data
            ?.filter((s) => s.topology_id === topology.id)
            .map((s) => (
              <div className="scenario-event" key={s.id}>
                <div>
                  <strong>{s.scenario_id}</strong>
                  <span>
                    {s.ground_truth} · {new Date(s.timestamp).toLocaleString()}
                  </span>
                </div>
                <span className="badge">{s.state}</span>
                {s.state === "active" && (
                  <Button
                    size="sm"
                    onClick={() =>
                      void mutation(`/scenarios/${s.id}/stop`).catch(() => {})
                    }
                  >
                    Stop event
                  </Button>
                )}
                <Button
                  size="icon"
                  title="Export scenario event"
                  onClick={() => downloadJSON(s, `scenario-${s.id}.json`)}
                >
                  <Download size={13} />
                </Button>
              </div>
            ))}
          {!scenarioEvents.data?.length && (
            <p className="help-text">No synthetic scenario events recorded.</p>
          )}
        </section>
      </div>
    );
  if (page === "Experiment Manager")
    return (
      <div className="page-content">
        <PageTitle
          eyebrow="PERSISTENT RESEARCH EVIDENCE"
          title="Experiment Manager"
          description="Immutable snapshots of topology, applied configuration, actual measurements and scenario ground truth."
        >
          <Button variant="default" onClick={p.capture}>
            <Plus size={14} /> Capture experiment
          </Button>
        </PageTitle>
        <div className="info-strip">
          Snapshots include evidence available at capture time. Subscriber
          credentials are omitted from all exports.
        </div>
        {experiments.error && (
          <div className="validation-error">
            Experiment list unavailable: {String(experiments.error)}
            <Button onClick={() => void experiments.refetch()}>Retry</Button>
          </div>
        )}
        <div className="export-toolbar">
          <span>
            {experiments.isPending
              ? "Loading snapshots…"
              : `${experiments.data?.length ?? 0} saved experiments`}
          </span>
          <label>
            Export format{" "}
            <select value={format} onChange={(e) => setFormat(e.target.value)}>
              <option value="zip">ZIP artifact archive</option>
              <option value="json">JSON evidence</option>
              <option value="csv">CSV measurements</option>
            </select>
          </label>
        </div>
        <div className="experiment-list">
          {experiments.data
            ?.slice()
            .reverse()
            .map((e) => (
              <article className="experiment-card" key={e.id}>
                <div className="experiment-icon">
                  <FolderOpen size={22} />
                </div>
                <div>
                  <h3>{e.name}</h3>
                  <p>{e.topology.name}</p>
                  <code>{e.id}</code>
                </div>
                <div>
                  <span className="badge">{e.category}</span>
                  <p>{new Date(e.timestamp).toLocaleString()}</p>
                </div>
                <Button
                  onClick={async () => {
                    try {
                      await p.save();
                      const restored = (await p.act(
                        `/experiments/${e.id}/restore`,
                      )) as import("@/lib/studio").Topology;
                      useStudio.getState().load(restored);
                      p.go("Network Builder");
                      p.notify(
                        "Experiment design restored into a fresh stopped workspace. Source evidence preserved.",
                      );
                    } catch {}
                  }}
                >
                  Restore design
                </Button>
                <a
                  className="button button-secondary"
                  href={`/api/experiments/${e.id}/export?format=${format}`}
                >
                  <Download size={14} /> Export
                </a>
              </article>
            ))}
          {experiments.isSuccess && !experiments.data?.length && (
            <div className="empty-state">
              <FlaskConical size={36} />
              <h3>Keep an evidence trail</h3>
              <p>
                Capture an experiment to preserve your design, runtime evidence
                and synthetic scenario labels.
              </p>
              <Button onClick={p.capture}>Capture first experiment</Button>
            </div>
          )}
        </div>
      </div>
    );
  if (page === "Logs & Diagnostics")
    return (
      <div className="page-content">
        <PageTitle
          eyebrow="AUDIT TRAIL"
          title="Logs & events"
          description="Lifecycle actions, configuration saves and scenario evidence. Live process output is in the bottom diagnostics panel."
        />
        <section className="surface">
          <div className="event-list">
            {events.data
              ?.slice()
              .reverse()
              .map((e) => (
                <article className="audit-event" key={e.id}>
                  <span className="event-type">{e.type}</span>
                  <div>
                    <strong>{e.message}</strong>
                    <details>
                      <summary>Inspect evidence</summary>
                      <pre>{JSON.stringify(e, null, 2)}</pre>
                    </details>
                  </div>
                  <time>{new Date(e.timestamp).toLocaleString()}</time>
                </article>
              ))}
            {!events.data?.length && (
              <p className="help-text">No events have been recorded.</p>
            )}
          </div>
        </section>
      </div>
    );
  if (page === "Configuration Files")
    return (
      <div className="page-content">
        <PageTitle
          eyebrow="CONFIGURATION PERSISTENCE"
          title="Configuration manager"
          description="Save or reopen topology documents. Generated run configs remain separate from upstream examples."
        >
          <a
            className="button button-secondary"
            href={`/api/configurations/${topology.id}/export`}
          >
            <Download size={14} /> Export current topology
          </a>
        </PageTitle>
        <section className="surface">
          <h3>Current project</h3>
          <label className="field">
            <span>Project name</span>
            <input
              value={topology.name}
              onChange={(e) =>
                edit((t) => {
                  t.name = e.target.value;
                })
              }
            />
          </label>
          <div className="keyvalue">
            <span>Saved revision</span>
            <code>{topology.revision}</code>
          </div>
          <Button onClick={() => p.save().catch(() => {})}>Save project</Button>
        </section>
        <section className="surface">
          <h3>Saved topology documents</h3>
          {savedTopologies.data?.map((t) => (
            <div className="saved-project" key={t.id}>
              <FileJson size={20} />
              <div>
                <strong>{t.name}</strong>
                <small>
                  {t.nodes.length} components · revision {t.revision}
                </small>
              </div>
              <Button
                onClick={async () => {
                  await p.save();
                  const loaded = await api<import("@/lib/studio").Topology>(
                    `/topologies/${t.id}`,
                  );
                  useStudio.getState().load(loaded);
                  p.go("Network Builder");
                }}
              >
                Open
              </Button>
              <a
                href={`/api/configurations/${t.id}/export`}
                className="button button-ghost"
              >
                Export
              </a>
            </div>
          ))}
        </section>
      </div>
    );
  if (page === "Settings")
    return (
      <div className="page-content">
        <PageTitle
          eyebrow="LOCAL ENGINE SETTINGS"
          title="Engine & capabilities"
          description="Read-only environment discovery. Studio does not replace or rebuild the existing LTE installation."
        />
        <section className="surface">
          <h3>Installed environment</h3>
          <div className="keyvalue">
            <span>Repository</span>
            <code>{p.system?.repository ?? "Unavailable"}</code>
          </div>
          <div className="keyvalue">
            <span>Source commit</span>
            <code>{p.system?.commit ?? "Unavailable"}</code>
          </div>
          <CheckRow
            ok={!!p.system?.zeromq_enabled}
            text="ZeroMQ enabled in CMake build"
          />
          <CheckRow ok={!p.system?.uhd_enabled} text="UHD disabled" />
          <CheckRow
            ok={!!p.system?.tun_available}
            text="TUN device available"
          />
          <CheckRow
            ok={!!p.system?.isolation_available}
            text="Unprivileged Linux namespace isolation"
          />
          {!p.system?.isolation_available && (
            <pre className="error-text">{p.system?.isolation_diagnostic}</pre>
          )}
          {Object.entries(p.system?.binaries ?? {}).map(([k, v]) => (
            <div className="keyvalue" key={k}>
              <span>{k}</span>
              <code>
                {v.path} · {v.executable ? "executable" : "missing"}
              </code>
            </div>
          ))}
        </section>
        <section className="surface">
          <h3>Execution capabilities</h3>
          <div className="capability-row">
            <span>One EPC + one eNodeB + one UE</span>
            <span className="badge cyan-text">REAL ADAPTER</span>
          </div>
          <div className="capability-row">
            <span>Topology metadata / scenario labels</span>
            <span className="badge success">LIVE</span>
          </div>
          <div className="capability-row">
            <span>Radio / TAC / transport / AWGN changes</span>
            <span className="badge amber-text">RESTART REQUIRED</span>
          </div>
          <div className="capability-row">
            <span>Two/three co-channel cells / native I/Q broker</span>
            <span className="badge success">NATIVE ADAPTER</span>
          </div>
          <div className="capability-row">
            <span>Handover / scheduled mobility</span>
            <span className="badge amber-text">NOT INTEGRATED</span>
          </div>
          <div className="capability-row">
            <span>Pause / arbitrary commands / over-the-air RF</span>
            <span className="badge">UNSUPPORTED</span>
          </div>
          <p className="help-text">
            Control API binds to 127.0.0.1. A local session cookie and
            Origin/Host checks protect state changes. User/network/mount
            namespaces isolate LTE interfaces and processes. No host-root helper
            is installed.
          </p>
        </section>
      </div>
    );
  return <CapabilityMatrix />;
}

function CheckRow({
  ok,
  text,
  passText = "Passed",
  failText = "Needs attention",
}: {
  ok: boolean;
  text: string;
  passText?: string;
  failText?: string;
}) {
  return (
    <div className={`check-row ${ok ? "ok" : ""}`}>
      {ok ? <CheckCircle2 size={15} /> : <XCircle size={15} />}
      <span>{text}</span>
      <span>{ok ? passText : failText}</span>
    </div>
  );
}
