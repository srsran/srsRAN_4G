"use client";
import { useCallback, useEffect, useRef, useState } from "react";
import {
  QueryClient,
  QueryClientProvider,
  useQuery,
  useQueryClient,
} from "@tanstack/react-query";
import { ReactFlowProvider } from "@xyflow/react";
import { AnimatePresence, motion } from "framer-motion";
import {
  Activity,
  ArrowLeftToLine,
  ArrowRightFromLine,
  Bell,
  Cable,
  Check,
  ChevronDown,
  ChevronRight,
  CircleHelp,
  Columns2,
  Cpu,
  Download,
  FileJson,
  FlaskConical,
  FolderOpen,
  Gauge,
  HardDrive,
  LayoutDashboard,
  ListChecks,
  Loader2,
  Logs,
  Maximize2,
  Minus,
  Network,
  PanelBottom,
  Play,
  Plus,
  RadioTower,
  Redo2,
  Save,
  Settings,
  Settings2,
  Shield,
  ShieldCheck,
  Square,
  Terminal,
  Undo2,
  Upload,
  X,
} from "lucide-react";
import {
  api,
  initialTopology,
  useStudio,
  type Log,
  type Metric,
  type Status,
  type System,
  type Topology,
  type Validation,
} from "@/lib/studio";
import { Button } from "./ui/button";
import { TopologyCanvas } from "./topology";
import { Inspector } from "./inspector";
import { WorkspacePage } from "./pages";
const NAV = [
  { label: "Capability Matrix", Icon: LayoutDashboard },
  { label: "Network Builder", Icon: Network },
  { label: "Component Inventory", Icon: RadioTower },
  { label: "Simulation Runtime", Icon: Gauge },
  { label: "Cell Configuration", Icon: RadioTower },
  { label: "Radio Environment", Icon: Network },
  { label: "Mobility Experiments", Icon: FlaskConical },
  { label: "ZeroMQ / RF Transport", Icon: Cable },
  { label: "Native Measurements", Icon: Activity },
  { label: "Logs & Diagnostics", Icon: Logs },
  { label: "Experiment Manager", Icon: FolderOpen },
  { label: "Configuration Files", Icon: FileJson },
  { label: "Settings", Icon: Settings },
];
const queryClient = new QueryClient({
  defaultOptions: {
    queries: { retry: 1, refetchOnWindowFocus: false, staleTime: 1000 },
  },
});
export default function Studio() {
  return (
    <QueryClientProvider client={queryClient}>
      <ReactFlowProvider>
        <StudioWorkspace />
      </ReactFlowProvider>
    </QueryClientProvider>
  );
}
function StudioWorkspace() {
  const qc = useQueryClient();
  const { topology, dirty, past, future } = useStudio();
  const [ready, setReady] = useState(false);
  const [connected, setConnected] = useState(false);
  const [page, setPage] = useState("Network Builder");
  const [sidebar, setSidebar] = useState(true);
  const [inspector, setInspector] = useState(true);
  const [layer, setLayer] = useState("Designed");
  const [toast, setToast] = useState("");
  const [busy, setBusy] = useState("");
  const [validation, setValidation] = useState<Validation>();
  const [logHeight, setLogHeight] = useState(205);
  const [logTab, setLogTab] = useState("All output");
  const [logs, setLogs] = useState<Log[]>([]);
  const [dialog, setDialog] = useState<"new" | "experiment" | null>(null);
  const [name, setName] = useState("");
  const [category, setCategory] = useState("Normal");
  const [search, setSearch] = useState("");
  const [projectMenu, setProjectMenu] = useState(false);
  const importRef = useRef<HTMLInputElement>(null);
  const sessionRef = useRef(false);
  const logRef = useRef<HTMLDivElement>(null);
  const notify = useCallback((s: string) => setToast(s), []);
  const connect = useCallback(async () => {
    try {
      await api("/session", "POST");
      setReady(true);
      const saved = await api<Topology[]>("/topologies");
      if (!sessionRef.current) {
        if (saved.length)
          useStudio
            .getState()
            .load(
              saved.find(
                (t) => t.id === localStorage.getItem("roguard.currentProject"),
              ) ??
                saved.find((t) => t.id === "default") ??
                saved[0],
            );
        sessionRef.current = true;
      }
      setLogs(await api<Log[]>("/logs"));
    } catch (e) {
      notify(
        "Engine unavailable. Start the backend on 127.0.0.1:8000. " + String(e),
      );
    }
  }, [notify]);
  useEffect(() => {
    void connect();
  }, [connect]);
  useEffect(() => {
    if (sessionRef.current)
      localStorage.setItem("roguard.currentProject", topology.id);
  }, [topology.id]);
  const system = useQuery({
    queryKey: ["system"],
    queryFn: () => api<System>("/system"),
    enabled: ready,
    refetchInterval: 4000,
  });
  const status = useQuery({
    queryKey: ["status"],
    queryFn: () => api<Status>("/simulations"),
    enabled: ready,
    refetchInterval: 2000,
  });
  const metrics = useQuery({
    queryKey: ["metrics", status.data?.run?.id],
    queryFn: () =>
      api<Metric[]>(
        "/metrics" +
          (status.data?.run?.id ? "?run_id=" + status.data.run.id : ""),
      ),
    enabled: ready,
    refetchInterval: 5000,
  });
  const events = useQuery({
    queryKey: ["events"],
    queryFn: () => api<Log[]>("/events"),
    enabled: ready,
    refetchInterval: 4000,
  });
  useEffect(() => {
    if (!ready) return;
    let ws: WebSocket;
    let timer: ReturnType<typeof setTimeout>;
    let disposed = false;
    let pendingLogs: Log[] = [];
    const flush = setInterval(() => {
      if (pendingLogs.length) {
        const batch = pendingLogs;
        pendingLogs = [];
        setLogs((old) => [...old, ...batch].slice(-1000));
      }
    }, 200);
    const open = () => {
      ws = new WebSocket(
        `${window.location.protocol === "https:" ? "wss:" : "ws:"}//${window.location.host}/api/ws`,
      );
      ws.onopen = () => setConnected(true);
      ws.onclose = () => {
        setConnected(false);
        if (!disposed) timer = setTimeout(open, 3000);
      };
      ws.onerror = () => setConnected(false);
      ws.onmessage = (e) => {
        let m;
        try {
          m = JSON.parse(e.data);
        } catch {
          return;
        }
        if (m.type === "log") {
          pendingLogs.push(m);
          if (pendingLogs.length > 1000) pendingLogs = pendingLogs.slice(-1000);
        }
        if (m.type === "status") qc.setQueryData(["status"], m.status);
        if (m.type === "metric")
          qc.setQueryData<Metric[]>(["metrics", m.run_id], (old) =>
            [...(old ?? []).filter((row) => row.id !== m.id), m].slice(-1000),
          );
        if (m.type === "failure") {
          notify(m.message);
          void qc.invalidateQueries({ queryKey: ["status"] });
        }
        if (["lifecycle", "attachment", "component"].includes(m.type))
          void qc.invalidateQueries({ queryKey: ["status"] });
      };
    };
    open();
    return () => {
      disposed = true;
      clearTimeout(timer);
      clearInterval(flush);
      ws?.close();
    };
  }, [ready, qc, notify]);
  useEffect(() => {
    if (!toast) return;
    const timer = setTimeout(() => setToast(""), 8000);
    return () => clearTimeout(timer);
  }, [toast]);
  useEffect(() => {
    if (logRef.current) logRef.current.scrollTop = logRef.current.scrollHeight;
  }, [logs, logTab]);
  const save = useCallback(async () => {
    const t = useStudio.getState().topology;
    try {
      const saved = await api<Topology>(`/topologies/${t.id}`, "PUT", t);
      useStudio.getState().saved(saved, t);
      await qc.invalidateQueries({ queryKey: ["topologies"] });
      return saved;
    } catch (e) {
      notify(String(e));
      throw e;
    }
  }, [qc, notify]);
  const act = useCallback(
    async (path: string, body?: unknown) => {
      try {
        const result = await api(path, "POST", body);
        await qc.invalidateQueries();
        return result;
      } catch (e) {
        notify(String(e));
        throw e;
      }
    },
    [qc, notify],
  );
  const runAction = useCallback(
    async (action: "validate" | "start" | "restart" | "stop") => {
      if (busy) return;
      setBusy(action);
      try {
        if (action === "stop") {
          await act("/simulations/stop");
          notify("Owned simulation shutdown completed.");
        } else {
          const t = await save();
          const report = await api<Validation>(
            `/topologies/${t.id}/validate`,
            "POST",
          );
          setValidation(report);
          if (action === "validate") {
            notify(
              report.runnable
                ? "Configuration and runtime prerequisites passed."
                : !report.valid
                  ? "Configuration has validation errors."
                  : !report.adapter_supported
                    ? "Design valid. The current native adapter cannot execute this design."
                    : "Configuration valid. Runtime prerequisites need attention.",
            );
            setPage("Simulation Runtime");
          } else if (!report.runnable) {
            setPage("Simulation Runtime");
            notify(
              "Start blocked by validation. Review configuration and environment diagnostics.",
            );
          } else {
            await act(`/simulations/${t.id}/${action}`);
            notify(
              "Real LTE startup requested. Waiting for process readiness and UE attachment.",
            );
          }
        }
      } catch {
      } finally {
        setBusy("");
      }
    },
    [busy, save, act, notify],
  );
  useEffect(() => {
    const handler = (e: KeyboardEvent) => {
      const target = e.target as HTMLElement;
      if (
        ["INPUT", "TEXTAREA", "SELECT"].includes(target.tagName) ||
        target.isContentEditable ||
        dialog
      )
        return;
      const mod = e.metaKey || e.ctrlKey;
      const s = useStudio.getState();
      if (mod && e.key.toLowerCase() === "s") {
        e.preventDefault();
        void save()
          .then(() => notify("Project saved."))
          .catch(() => {});
      } else if (
        ["Network Builder", "Cell Configuration"].includes(page) &&
        layer === "Designed"
      ) {
        if (mod && e.key.toLowerCase() === "z") {
          e.preventDefault();
          e.shiftKey ? s.redo() : s.undo();
        } else if (mod && e.key.toLowerCase() === "y") {
          e.preventDefault();
          s.redo();
        } else if (mod && e.key.toLowerCase() === "c") {
          e.preventDefault();
          s.copy();
        } else if (mod && e.key.toLowerCase() === "v") {
          e.preventDefault();
          s.paste();
        } else if (mod && e.key.toLowerCase() === "d") {
          e.preventDefault();
          s.duplicate();
        } else if (e.key === "Delete" || e.key === "Backspace") {
          e.preventDefault();
          s.remove();
        }
      }
    };
    window.addEventListener("keydown", handler);
    return () => window.removeEventListener("keydown", handler);
  }, [page, layer, dialog, save, notify]);
  useEffect(() => {
    const warn = (e: BeforeUnloadEvent) => {
      if (useStudio.getState().dirty) e.preventDefault();
    };
    window.addEventListener("beforeunload", warn);
    return () => window.removeEventListener("beforeunload", warn);
  }, []);
  const resize = (e: React.PointerEvent) => {
    e.currentTarget.setPointerCapture(e.pointerId);
    const start = e.clientY,
      height = logHeight;
    const move = (event: PointerEvent) =>
      setLogHeight(
        Math.max(
          85,
          Math.min(window.innerHeight * 0.65, height + start - event.clientY),
        ),
      );
    const end = () => {
      window.removeEventListener("pointermove", move);
      window.removeEventListener("pointerup", end);
    };
    window.addEventListener("pointermove", move);
    window.addEventListener("pointerup", end);
  };
  const selectedLogs =
    logTab === "Validation"
      ? validation
        ? [
            ...validation.errors,
            ...(validation.adapter_blockers ?? []),
            ...validation.runtime_blockers,
            ...validation.warnings,
          ].map((message) => ({
            type: "validation",
            timestamp: "",
            component: "validation",
            message,
          }))
        : []
      : logTab === "System events"
        ? (events.data ?? [])
        : logTab === "ZeroMQ"
          ? logs.filter(
              (l) =>
                l.component === "broker" ||
                /zmq|socket|port|rf /i.test(l.message),
            )
          : logs.filter(
              (l) =>
                logTab === "All output" ||
                (logTab === "eNB"
                  ? l.component?.startsWith("enb")
                  : l.component === logTab.toLowerCase()),
            );
  const filteredLogs = selectedLogs.filter(
    (l) => !search || l.message.toLowerCase().includes(search.toLowerCase()),
  );
  const runtime =
    status.isError || !ready ? "unknown" : (status.data?.state ?? "unknown");
  const active = ["running", "starting", "stopping"].includes(runtime);
  const seconds = Math.round(status.data?.runtime_seconds ?? 0);
  const uptime = `${String(Math.floor(seconds / 3600)).padStart(2, "0")}:${String(Math.floor(seconds / 60) % 60).padStart(2, "0")}:${String(seconds % 60).padStart(2, "0")}`;
  const capture = () => {
    setName(`LTE experiment · ${new Date().toLocaleDateString()}`);
    setDialog("experiment");
  };
  return (
    <div className={`studio-app ${sidebar ? "" : "sidebar-collapsed"}`}>
      <aside className="navigation">
        <a className="brand" href="/" aria-label="srsRAN Studio">
          <span className="brand-mark">
            <Shield size={23} strokeWidth={1.6} />
            <i />
          </span>
          {sidebar && (
            <div>
              <strong>
                srsRAN<span>STUDIO</span>
              </strong>
              <small>LTE SIMULATION PLATFORM</small>
            </div>
          )}
        </a>
        <div className="nav-section-label">WORKSPACE</div>
        <nav>
          {NAV.map(({ label, Icon }, i) => (
            <button
              key={label}
              title={label}
              className={`${page === label ? "active" : ""} ${i === 8 ? "nav-divider" : ""}`}
              onClick={() => {
                if (label === "Cell Configuration") {
                  const cell = topology.nodes.flatMap((n) => n.cells ?? [])[0];
                  if (cell) useStudio.getState().setSelected([cell.id]);
                }
                setPage(label);
              }}
            >
              <Icon size={17} strokeWidth={1.6} />
              {sidebar && <span>{label}</span>}
              {page === label && sidebar && (
                <span className="nav-active-mark" />
              )}
            </button>
          ))}
        </nav>
        <div className="nav-bottom">
          {sidebar && (
            <div className="lab-card">
              <span>
                <ShieldCheck size={14} /> ISOLATED LABORATORY
              </span>
              <p>
                Synthetic subscribers
                <br />
                ZeroMQ virtual radio
              </p>
              <div>
                <i className={`state-dot ${system.isSuccess ? "green" : ""}`} />
                {system.isSuccess
                  ? "Local engine available"
                  : "Engine disconnected"}
              </div>
            </div>
          )}
          <button
            className="collapse-button"
            onClick={() => setSidebar(!sidebar)}
            title={sidebar ? "Collapse sidebar" : "Expand sidebar"}
          >
            {sidebar ? (
              <ArrowLeftToLine size={16} />
            ) : (
              <ArrowRightFromLine size={16} />
            )}{" "}
            {sidebar && <span>Collapse workspace</span>}
          </button>
          <div className="workspace-user">
            <div>RG</div>
            {sidebar && (
              <span>
                LTE research<small>Local workspace</small>
              </span>
            )}
          </div>
        </div>
      </aside>
      <main className="main-shell">
        <header className="topbar">
          <div className="project-switcher">
            <button onClick={() => setProjectMenu(!projectMenu)}>
              <span className="project-icon">
                <Network size={17} />
              </span>
              <div>
                <small>CURRENT PROJECT</small>
                <strong>{topology.name}</strong>
              </div>
              <ChevronDown size={13} />
              {dirty && (
                <i className="unsaved-dot" title="Unsaved design changes" />
              )}
            </button>
            {projectMenu && (
              <div className="project-dropdown">
                <button
                  onClick={() => {
                    setName("New LTE laboratory");
                    setDialog("new");
                    setProjectMenu(false);
                  }}
                >
                  <Plus size={14} />
                  New project
                </button>
                <button
                  onClick={() => {
                    setPage("Configuration Files");
                    setProjectMenu(false);
                  }}
                >
                  <FolderOpen size={14} />
                  Open saved project
                </button>
                <button
                  onClick={() => {
                    importRef.current?.click();
                    setProjectMenu(false);
                  }}
                >
                  <Upload size={14} />
                  Import topology JSON
                </button>
              </div>
            )}
          </div>
          <div className="topbar-status">
            <span className="mode-badge">
              ZMQ <i /> NATIVE LTE
            </span>
            <button
              className="engine-connection"
              onClick={() => void connect()}
              title="Reconnect to local engine"
            >
              <i
                className={`state-dot ${connected ? "green" : system.isSuccess ? "amber" : ""}`}
              />
              {connected
                ? "Engine connected"
                : system.isSuccess
                  ? "REST connected"
                  : "Engine offline"}
            </button>
            <span className="runtime-clock mono">
              {runtime === "unknown" ? "—" : uptime}
            </span>
            <span title="Actual host CPU utilization" className="resource">
              <Cpu size={13} />
              {system.data?.resources.cpu_percent.toFixed(0) ?? "—"}%
            </span>
            <span title="Actual host memory utilization" className="resource">
              <HardDrive size={13} />
              {system.data?.resources.memory_percent.toFixed(0) ?? "—"}%
            </span>
          </div>
          <div className="topbar-actions">
            <Button
              disabled={!!busy || !ready}
              onClick={() => {
                setBusy("save");
                void save()
                  .then(() => notify("Project saved."))
                  .catch(() => {})
                  .finally(() => setBusy(""));
              }}
            >
              <Save size={14} />
              <span>Save project</span>
            </Button>
            <Button
              variant="default"
              disabled={!!busy || active || !ready}
              onClick={() => void runAction("start")}
            >
              {busy === "start" ? (
                <Loader2 size={14} className="spin" />
              ) : (
                <Play size={13} fill="currentColor" />
              )}
              <span>Start simulation</span>
            </Button>
            <Button
              size="icon"
              disabled={!!busy || !active}
              onClick={() => void runAction("stop")}
              title="Stop owned LTE processes"
            >
              <Square size={13} />
            </Button>
          </div>
        </header>
        <div className="workspace-heading">
          <div>
            <div className="heading-icon">
              {["Network Builder", "Cell Configuration"].includes(page) ? (
                <Network size={20} />
              ) : (
                <Activity size={20} />
              )}
            </div>
            <div>
              <h1>{page}</h1>
              <p>
                {["Network Builder", "Cell Configuration"].includes(page)
                  ? "Design, connect and operate your LTE laboratory"
                  : "srsRAN Studio · local LTE workbench"}
              </p>
            </div>
          </div>
          <div className="workspace-heading-actions">
            <span
              data-run-id={status.data?.run?.id ?? ""}
              className={`runtime-indicator ${runtime === "running" ? "running" : ""}`}
            >
              <i
                className={`state-dot ${runtime === "running" ? "green" : runtime === "failed" ? "red" : ""}`}
              />
              {runtime === "unknown"
                ? "Engine unavailable"
                : runtime === "running"
                  ? "Simulation running"
                  : runtime === "starting"
                    ? "Waiting for readiness"
                    : runtime === "failed"
                      ? "Simulation failed"
                      : "Simulation " + runtime}
            </span>
            <Button variant="ghost" onClick={capture}>
              <FlaskConical size={14} /> Capture experiment
            </Button>
          </div>
        </div>
        <div className="workspace-tabbar">
          <div className="workspace-layer-tabs">
            {["Network Builder", "Cell Configuration"].includes(page) ? (
              ["Designed", "Applied", "Observed"].map((l) => (
                <button
                  key={l}
                  onClick={() => setLayer(l)}
                  className={layer === l ? "active" : ""}
                >
                  <span className={`tiny-square ${l.toLowerCase()}`} />
                  {l} topology{l === "Designed" && dirty && <i />}
                </button>
              ))
            ) : (
              <span className="workspace-tab-label">{page.toUpperCase()}</span>
            )}
          </div>
          <div className="workspace-edit-tools">
            <Button
              size="icon"
              title="Undo · Ctrl+Z"
              disabled={!past.length || layer !== "Designed"}
              onClick={() => useStudio.getState().undo()}
            >
              <Undo2 size={14} />
            </Button>
            <Button
              size="icon"
              title="Redo · Ctrl+Shift+Z"
              disabled={!future.length || layer !== "Designed"}
              onClick={() => useStudio.getState().redo()}
            >
              <Redo2 size={14} />
            </Button>
            <span className="tool-divider" />
            <Button
              onClick={() => void runAction("validate")}
              disabled={!!busy || !ready}
            >
              <ShieldCheck size={14} /> Validate
            </Button>
            <Button
              size="icon"
              title="Toggle inspector"
              onClick={() => setInspector(!inspector)}
            >
              <Columns2 size={15} />
            </Button>
            <Button
              size="icon"
              title="Toggle diagnostics"
              onClick={() => setLogHeight(logHeight ? 0 : 205)}
            >
              <PanelBottom size={15} />
            </Button>
          </div>
        </div>
        {(dirty || status.data?.restart_required) && (
          <div className="pending-strip">
            <span className="state-dot amber" />
            {dirty
              ? "Unsaved designed topology."
              : "Configuration saved — restart required."}
            <span>Applied runtime stays at its recorded configuration.</span>
          </div>
        )}
        <div className="workspace-main">
          {["Network Builder", "Cell Configuration"].includes(page) ? (
            <>
              <TopologyCanvas
                status={status.isError ? undefined : status.data}
                notify={notify}
                layer={layer}
              />
              {inspector && (
                <Inspector
                  status={status.isError ? undefined : status.data}
                  validation={validation}
                  onRestart={() => {
                    setPage("Simulation Runtime");
                    void save().catch(() => {});
                  }}
                />
              )}
            </>
          ) : (
            <WorkspacePage
              busy={!!busy}
              page={page}
              status={status.isError ? undefined : status.data}
              system={system.data}
              validation={validation}
              metrics={metrics.data ?? []}
              act={act}
              save={save}
              validate={() => void runAction("validate")}
              start={() => void runAction("start")}
              stop={() => void runAction("stop")}
              restart={() => void runAction("restart")}
              notify={notify}
              go={setPage}
              capture={capture}
            />
          )}
        </div>
        {logHeight > 0 && (
          <section className="diagnostics" style={{ height: logHeight }}>
            <div
              className="panel-resizer"
              role="separator"
              aria-label="Resize diagnostics"
              aria-orientation="horizontal"
              tabIndex={0}
              onPointerDown={resize}
              onKeyDown={(e) => {
                if (e.key === "ArrowUp")
                  setLogHeight((v) => Math.min(v + 30, 500));
                if (e.key === "ArrowDown")
                  setLogHeight((v) => Math.max(v - 30, 85));
              }}
            />
            <div className="diagnostics-header">
              <div className="diagnostics-tabs">
                <Terminal size={14} />
                {[
                  "All output",
                  "eNB",
                  "UE",
                  "EPC",
                  "ZeroMQ",
                  "System events",
                  "Validation",
                ].map((t) => (
                  <button
                    className={logTab === t ? "active" : ""}
                    key={t}
                    onClick={() => setLogTab(t)}
                  >
                    {t}
                  </button>
                ))}
              </div>
              <div className="diagnostics-actions">
                <input
                  aria-label="Filter diagnostics"
                  placeholder="Filter output…"
                  value={search}
                  onChange={(e) => setSearch(e.target.value)}
                />
                <span className="mono">
                  {filteredLogs.length > 300 ? "Latest 300 / " : ""}
                  {filteredLogs.length} entries
                </span>
                <button
                  aria-label="Expand diagnostics"
                  onClick={() => setLogHeight(logHeight > 300 ? 205 : 420)}
                >
                  <Maximize2 size={13} />
                </button>
                <button
                  aria-label="Hide diagnostics"
                  onClick={() => setLogHeight(0)}
                >
                  <Minus size={15} />
                </button>
              </div>
            </div>
            <div ref={logRef} className="log-output">
              {filteredLogs.length ? (
                filteredLogs.slice(-300).map((l, i) => (
                  <div
                    className={`log-line ${l.type === "failure" ? "failure" : ""}`}
                    key={i}
                  >
                    <time>
                      {l.timestamp
                        ? new Date(l.timestamp).toLocaleTimeString("en-GB", {
                            hour12: false,
                          })
                        : "—"}
                    </time>
                    <span className="log-component">
                      {l.component ?? l.type}
                    </span>
                    <span>{l.message}</span>
                  </div>
                ))
              ) : (
                <div className="log-empty">
                  <Terminal size={22} />
                  <div>
                    <strong>
                      {logTab === "Validation"
                        ? "No validation results yet"
                        : "Waiting for actual engine output"}
                    </strong>
                    <span>
                      {logTab === "Validation"
                        ? "Save and validate the topology to inspect diagnostics."
                        : "Process logs appear after a real simulation starts. System events are recorded separately."}
                    </span>
                  </div>
                </div>
              )}
            </div>
          </section>
        )}
        <footer className="statusbar">
          <div>
            <span className="statusbar-brand">srsRAN</span>
            <span>LTE Simulation Studio</span>
            <i />
            <span>{ready ? "Local control API" : "API unavailable"}</span>
            <span className="mono">/api · local proxy</span>
          </div>
          <div>
            <span>2D topology</span>
            <i />
            <span>Native I/Q engine adapter</span>
            <i />
            <span className="mono">
              {topology.nodes.length} NODES / {topology.links.length} LINKS
            </span>
          </div>
        </footer>
      </main>
      <input
        ref={importRef}
        type="file"
        accept=".json,application/json"
        hidden
        onChange={async (e) => {
          const file = e.target.files?.[0];
          if (!file) return;
          try {
            const imported = JSON.parse(await file.text()) as Topology;
            imported.id = crypto.randomUUID();
            imported.revision = 0;
            const saved = await api<Topology>(
              `/topologies/${imported.id}`,
              "PUT",
              imported,
            );
            if (dirty) await save();
            useStudio.getState().load(saved);
            setPage("Network Builder");
            setLayer("Designed");
            notify("Imported topology validated and saved.");
          } catch (err) {
            notify("Import failed: " + String(err));
          } finally {
            e.target.value = "";
          }
        }}
      />
      <AnimatePresence>
        {toast && (
          <motion.div
            role="status"
            className="toast"
            initial={{ opacity: 0, y: 15 }}
            animate={{ opacity: 1, y: 0 }}
            exit={{ opacity: 0, y: 10 }}
          >
            <InfoIcon />
            <span>{toast}</span>
            <button
              onClick={() => setToast("")}
              aria-label="Dismiss notification"
            >
              <X size={14} />
            </button>
          </motion.div>
        )}
      </AnimatePresence>
      {dialog && (
        <div className="modal-backdrop">
          <form
            className="modal"
            role="dialog"
            aria-modal="true"
            aria-labelledby="modal-title"
            onSubmit={async (e) => {
              e.preventDefault();
              setBusy("dialog");
              try {
                if (dialog === "new") {
                  if (dirty) await save();
                  useStudio.getState().load({
                    id: crypto.randomUUID(),
                    name,
                    nodes: [],
                    links: [],
                    revision: 0,
                  });
                  setLayer("Designed");
                  setPage("Network Builder");
                  setValidation(undefined);
                  notify(
                    "New empty laboratory. Add EPC, eNodeB and UE from the component library.",
                  );
                } else {
                  const t = await save();
                  await act("/experiments", {
                    name,
                    category,
                    topology_id: t.id,
                  });
                  notify("Experiment snapshot saved with available evidence.");
                  setPage("Experiment Manager");
                }
                setDialog(null);
              } catch {
              } finally {
                setBusy("");
              }
            }}
          >
            <button
              type="button"
              className="modal-close"
              aria-label="Close dialog"
              onClick={() => setDialog(null)}
            >
              <X size={17} />
            </button>
            <span className="eyebrow">
              {dialog === "new"
                ? "CREATE A LABORATORY"
                : "PRESERVE RESEARCH EVIDENCE"}
            </span>
            <h2 id="modal-title">
              {dialog === "new" ? "New LTE project" : "Capture experiment"}
            </h2>
            <p>
              {dialog === "new"
                ? "Start with an empty canvas and configure a real LTE topology."
                : "Save an immutable snapshot of the current design and all available runtime evidence."}
            </p>
            <label className="field">
              <span>{dialog === "new" ? "Project" : "Experiment"} name</span>
              <input
                autoFocus
                required
                minLength={1}
                maxLength={100}
                value={name}
                onChange={(e) => setName(e.target.value)}
              />
            </label>
            {dialog === "experiment" && (
              <label className="field">
                <span>Scenario ground truth · analyst supplied</span>
                <select
                  value={category}
                  onChange={(e) => setCategory(e.target.value)}
                >
                  <option>Normal</option>
                  <option>Legitimate Change</option>
                  <option>Suspicious</option>
                </select>
              </label>
            )}
            <div className="modal-actions">
              <Button type="button" onClick={() => setDialog(null)}>
                Cancel
              </Button>
              <Button variant="default" type="submit" disabled={!!busy}>
                {busy ? (
                  <Loader2 size={13} className="spin" />
                ) : dialog === "new" ? (
                  <Plus size={13} />
                ) : (
                  <Save size={13} />
                )}{" "}
                {dialog === "new" ? "Create project" : "Save snapshot"}
              </Button>
            </div>
          </form>
        </div>
      )}
    </div>
  );
}
function InfoIcon() {
  return <ShieldCheck size={18} />;
}
