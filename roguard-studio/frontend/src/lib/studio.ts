import { create } from "zustand";
export type Kind =
  | "epc"
  | "enb"
  | "ue"
  // Read-only compatibility with old runtime snapshots; never creation tools.
  | "suspicious"
  | "temporary"
  | "namespace"
  | "channel"
  | "event"
  | "sector"
  | "zmq";
export type Config = {
  mcc: string;
  mnc: string;
  enb_id: number;
  cell_id: number;
  tac: number;
  pci: number;
  earfcn: number;
  n_prb: number;
  tx_gain: number;
  rx_gain: number;
  base_srate: number;
  rf_driver: string;
  tx_port: number;
  rx_port: number;
  device_id: string;
  mme_addr: string;
  bind_addr: string;
  imsi: string;
  apn: string;
  log_level: string;
  metrics_enabled: boolean;
  report_json: boolean;
  namespace: string;
  awgn_enabled: boolean;
  awgn_snr: number;
};
export type CellConfig = {
  cell_id: number;
  pci: number;
  tac: number;
  dl_earfcn: number;
  ul_earfcn: number | null;
  rf_port: number;
  root_seq_idx: number;
  ho_active: boolean;
};
export type RFPlan = {
  adapter: string;
  sample_rate: number;
  ue_tx: number;
  broker_dl: number;
  paths: {
    cell_id: string;
    enb_id: string;
    role: string;
    rf_port: number;
    native_tx: number;
    broker_ul: number;
    attenuation_db: number;
  }[];
  processes: {
    role: string;
    component_id: string;
    config: string;
    metrics: string;
  }[];
  model: string;
};
export type Cell = {
  rf_attenuation_db?: number;
  id: string;
  name: string;
  config: CellConfig;
};
export const defaultCell = (id = crypto.randomUUID(), index = 0): Cell => ({
  id,
  rf_attenuation_db: index ? 20 : 0,
  name: `Cell ${String(index + 1).padStart(2, "0")}`,
  config: {
    cell_id: index + 1,
    pci: 1 + index * 5,
    tac: 7,
    dl_earfcn: 3350,
    ul_earfcn: null,
    rf_port: index,
    root_seq_idx: 128,
    ho_active: false,
  },
});
export type Component = {
  cells?: Cell[];
  id: string;
  kind: Kind;
  name: string;
  position: { x: number; y: number };
  config: Config;
  group?: string | null;
};
// Allocate a new identity without changing any operator-edited existing cell.
export const compatiblePci = (
  nodes: Component[],
  siblings: Cell[],
  preferred: number,
): number | undefined => {
  const used = new Set(
    nodes.flatMap((n) => n.cells ?? []).map((c) => c.config.pci),
  );
  const pss = new Set(siblings.map((c) => c.config.pci % 3));
  return [
    preferred,
    1,
    6,
    11,
    ...Array.from({ length: 504 }, (_, i) => i),
  ].find((pci) => !used.has(pci) && !pss.has(pci % 3));
};
export type Link = {
  id: string;
  source: string;
  target: string;
  kind: "s1" | "zmq" | "annotation";
};
export type MobilityPlan = {
  ue_id: string;
  start: { x: number; y: number };
  end: { x: number; y: number };
  speed: number;
  duration: number;
  mode: "planned";
};
export type Topology = {
  schema_version?: "srsran.studio.topology.v2";
  mobility?: MobilityPlan[];
  id: string;
  name: string;
  nodes: Component[];
  links: Link[];
  revision: number;
};
export type Metric = {
  id: string;
  timestamp: string;
  run_id: string;
  component: string;
  source: string;
  values: Record<string, number | null>;
};
export type Log = {
  timestamp: string;
  component?: string;
  message: string;
  type: string;
  run_id?: string;
};
export type Status = {
  node_components?: Record<
    string,
    {
      state: string;
      pid?: number;
      cpu_percent?: number;
      memory_bytes?: number;
      process_state?: string;
    }
  >;
  state: string;
  run: null | {
    id: string;
    topology: Topology;
    error?: string;
    rf_plan?: RFPlan | null;
    broker?: {
      data: {
        event: string;
        downlink_samples?: number;
        uplink_samples?: number;
        paths?: {
          input_samples: number;
          output_samples: number;
          input_energy: number;
          amplitude_gain: number;
        }[];
      };
    };
    started_at: string;
    ue_ip?: string;
    [key: string]: unknown;
  };
  components: Record<
    string,
    {
      state: string;
      pid?: number;
      cpu_percent?: number;
      memory_bytes?: number;
      process_state?: string;
    }
  >;
  attached: boolean;
  observed: null | {
    parameters: { pci: number; earfcn: number };
    source: string;
    timestamp: string;
  };
  serving_cell?: null | {
    cell_id: string;
    enb_id: string;
    name: string;
    pci: number;
    earfcn: number;
    provenance: string;
    timestamp: string;
  };
  restart_required: boolean;
  runtime_seconds?: number;
};
export type Validation = {
  rf_plan?: RFPlan | null;
  suggested_pci_changes?: {
    enb_id: string;
    cell_id: string;
    cell_name: string;
    from_pci: number;
    to_pci: number;
  }[];
  valid: boolean;
  environment_ready: boolean;
  adapter_supported: boolean;
  adapter_blockers: string[];
  runnable: boolean;
  errors: string[];
  runtime_blockers: string[];
  warnings: string[];
};
export type System = {
  repository: string;
  commit: string;
  zeromq_enabled: boolean;
  uhd_enabled: boolean;
  plugin: string | null;
  tun_available: boolean;
  isolation_available: boolean;
  isolation_diagnostic: string;
  binaries: Record<
    string,
    { path: string; exists: boolean; executable: boolean }
  >;
  resources: {
    cpu_percent: number;
    memory_percent: number;
    memory_used_bytes: number;
  };
  existing_config_paths: string[];
  multi_cell: string;
};
export const labels: Record<Kind, string> = {
  epc: "Evolved Packet Core",
  enb: "LTE eNodeB",
  ue: "User Equipment",
  suspicious: "Legacy eNodeB",
  temporary: "Legacy eNodeB",
  namespace: "Namespace group",
  channel: "Virtual channel",
  event: "Topology event",
  sector: "Cell / sector",
  zmq: "ZeroMQ annotation",
};
export const defaults = (kind: Kind): Config => ({
  mcc: "001",
  mnc: "01",
  enb_id: 411,
  cell_id: 1,
  tac: 7,
  pci: 1,
  earfcn: 3350,
  n_prb: 50,
  tx_gain: 80,
  rx_gain: 40,
  base_srate: 23040000,
  rf_driver: "zmq",
  tx_port: kind === "ue" ? 2001 : 2000,
  rx_port: kind === "ue" ? 2000 : 2001,
  device_id: kind === "ue" ? "ue" : "enb",
  mme_addr: "127.0.1.100",
  bind_addr: "127.0.1.1",
  imsi: "001010123456780",
  apn: "srsapn",
  log_level: "info",
  metrics_enabled: true,
  report_json: false,
  namespace: "rgstudio-ue",
  awgn_enabled: false,
  awgn_snr: 30,
});
export const initialTopology = (): Topology => ({
  id: "default",
  name: "LTE · Baseline laboratory",
  revision: 0,
  nodes: [
    {
      id: "epc-01",
      kind: "epc",
      name: "Core network",
      position: { x: 80, y: 180 },
      config: defaults("epc"),
    },
    {
      id: "enb-01",
      kind: "enb",
      name: "eNodeB · 01",
      position: { x: 440, y: 180 },
      config: defaults("enb"),
      cells: [defaultCell("cell-enb-01")],
    },
    {
      id: "ue-01",
      kind: "ue",
      name: "Laboratory UE",
      position: { x: 800, y: 180 },
      config: defaults("ue"),
    },
  ],
  links: [
    { id: "s1-01", source: "epc-01", target: "enb-01", kind: "s1" },
    { id: "rf-01", source: "enb-01", target: "ue-01", kind: "zmq" },
  ],
});
const copy = <T>(v: T): T => structuredClone(v);
type StudioState = {
  topology: Topology;
  selected: string[];
  dirty: boolean;
  past: Topology[];
  future: Topology[];
  clipboard: Component[];
  setSelected: (ids: string[]) => void;
  edit: (fn: (t: Topology) => void, history?: boolean) => void;
  load: (t: Topology) => void;
  saved: (t: Topology, requested: Topology) => void;
  undo: () => void;
  redo: () => void;
  addCell: (enbId: string) => void;
  add: (kind: Kind, position?: { x: number; y: number }) => void;
  remove: () => void;
  duplicate: () => void;
  copy: () => void;
  paste: () => void;
  group: () => void;
};
export const useStudio = create<StudioState>((set, get) => ({
  topology: initialTopology(),
  selected: ["enb-01"],
  dirty: false,
  past: [],
  future: [],
  clipboard: [],
  setSelected: (selected) =>
    set((s) =>
      s.selected.length === selected.length &&
      s.selected.every((id, i) => id === selected[i])
        ? s
        : { selected },
    ),
  edit: (fn, history = true) =>
    set((s) => {
      const t = copy(s.topology);
      fn(t);
      return {
        topology: t,
        dirty: true,
        past: history ? [...s.past.slice(-49), copy(s.topology)] : s.past,
        future: [],
      };
    }),
  load: (t) =>
    set({
      topology: copy(t),
      dirty: false,
      past: [],
      future: [],
      selected: [],
    }),
  saved: (t, requested) =>
    set((s) => {
      if (s.topology.id !== t.id) return {};
      return s.topology === requested
        ? { topology: copy(t), dirty: false }
        : { topology: { ...s.topology, revision: t.revision }, dirty: true };
    }),
  undo: () =>
    set((s) =>
      s.past.length
        ? {
            topology: {
              ...s.past[s.past.length - 1],
              revision: s.topology.revision,
            },
            past: s.past.slice(0, -1),
            future: [s.topology, ...s.future],
            dirty: true,
          }
        : {},
    ),
  redo: () =>
    set((s) =>
      s.future.length
        ? {
            topology: { ...s.future[0], revision: s.topology.revision },
            past: [...s.past, s.topology],
            future: s.future.slice(1),
            dirty: true,
          }
        : {},
    ),
  addCell: (enbId) => {
    let selected = "";
    get().edit((t) => {
      const enb = t.nodes.find((n) => n.id === enbId && n.kind === "enb");
      if (!enb || (enb.cells?.length ?? 0) >= 5) return;
      enb.cells ??= [];
      const cell = defaultCell(crypto.randomUUID(), enb.cells.length);
      const pci = compatiblePci(t.nodes, enb.cells, cell.config.pci);
      if (pci !== undefined) cell.config.pci = pci;
      else {
        const used = new Set(
          t.nodes.flatMap((n) => n.cells ?? []).map((c) => c.config.pci),
        );
        cell.config.pci =
          Array.from({ length: 504 }, (_, i) => i).find(
            (value) => !used.has(value),
          ) ?? cell.config.pci;
      }
      if (enb.cells[0]) {
        cell.config.tac = enb.cells[0].config.tac;
        cell.config.dl_earfcn = enb.cells[0].config.dl_earfcn;
        cell.config.ul_earfcn = enb.cells[0].config.ul_earfcn;
      }
      while (enb.cells.some((c) => c.config.cell_id === cell.config.cell_id))
        cell.config.cell_id++;
      enb.cells.push(cell);
      selected = cell.id;
    });
    if (selected) get().setSelected([selected]);
  },
  add: (kind, position) => {
    if (!["epc", "enb", "ue"].includes(kind)) return;
    const id = crypto.randomUUID();
    get().edit((t) => {
      const index = t.nodes.filter((n) => n.kind === kind).length;
      const config = defaults(kind);
      if (kind === "enb") {
        config.enb_id += index;
        config.bind_addr = `127.0.1.${index + 1}`;
        config.tx_port += index * 2;
        config.rx_port += index * 2;
        config.device_id = `enb${index + 1}`;
      }
      const node: Component = {
        id,
        kind,
        name: labels[kind] + ` · ${String(index + 1).padStart(2, "0")}`,
        position: position ?? {
          x: 80 + (t.nodes.length % 3) * 390,
          y: 110 + Math.floor(t.nodes.length / 3) * 390,
        },
        config,
        cells: kind === "enb" ? [defaultCell()] : [],
      };
      if (kind === "enb") {
        node.cells![0].config.pci =
          compatiblePci(t.nodes, [], (1 + index * 5) % 504) ??
          (1 + index * 5) % 504;
        node.cells![0].rf_attenuation_db = index ? 20 : 0;
      }
      t.nodes.push(node);
      // Explicit transport/core relationships are conveniences, never attachment evidence.
      const epcs = t.nodes.filter((n) => n.kind === "epc"),
        enbs = t.nodes.filter((n) => n.kind === "enb"),
        ues = t.nodes.filter((n) => n.kind === "ue");
      if (epcs.length === 1)
        for (const enb of enbs) {
          if (
            !t.links.some(
              (l) => l.kind === "s1" && [l.source, l.target].includes(enb.id),
            )
          )
            t.links.push({
              id: crypto.randomUUID(),
              source: epcs[0].id,
              target: enb.id,
              kind: "s1",
            });
        }
      if (ues.length === 1) {
        if (enbs.length === 1) {
          ues[0].config.tx_port = enbs[0].config.rx_port;
          ues[0].config.rx_port = enbs[0].config.tx_port;
        }
        for (const enb of enbs)
          if (
            !t.links.some(
              (l) =>
                l.kind === "zmq" &&
                [l.source, l.target].includes(enb.id) &&
                [l.source, l.target].includes(ues[0].id),
            )
          )
            t.links.push({
              id: crypto.randomUUID(),
              source: enb.id,
              target: ues[0].id,
              kind: "zmq",
            });
      }
    });
    get().setSelected([id]);
  },
  remove: () => {
    const selected = get().selected;
    get().edit((t) => {
      for (const n of t.nodes) {
        if (n.cells) {
          n.cells = n.cells.filter((c) => !selected.includes(c.id));
          n.cells.forEach((c, i) => {
            c.config.rf_port = i;
          });
        }
        if (n.group && selected.includes(n.group)) {
          const parent = t.nodes.find((p) => p.id === n.group);
          if (parent)
            n.position = {
              x: n.position.x + parent.position.x,
              y: n.position.y + parent.position.y,
            };
          n.group = null;
        }
      }
      t.mobility = t.mobility?.filter((m) => !selected.includes(m.ue_id));
      t.nodes = t.nodes.filter((n) => !selected.includes(n.id));
      const remainingEnbs = t.nodes.filter((n) => n.kind === "enb"),
        remainingUes = t.nodes.filter((n) => n.kind === "ue");
      if (remainingEnbs.length === 1 && remainingUes.length === 1) {
        remainingUes[0].config.tx_port = remainingEnbs[0].config.rx_port;
        remainingUes[0].config.rx_port = remainingEnbs[0].config.tx_port;
      }
      t.links = t.links.filter(
        (e) => !selected.includes(e.source) && !selected.includes(e.target),
      );
    });
    get().setSelected([]);
  },
  copy: () =>
    set((s) => ({
      clipboard: copy(
        s.topology.nodes.filter((n) => s.selected.includes(n.id)),
      ),
    })),
  paste: () => {
    const clipboard = get().clipboard;
    if (!clipboard.length) return;
    const ids = new Map(clipboard.map((n) => [n.id, crypto.randomUUID()]));
    get().edit((t) => {
      const links = t.links.filter(
        (e) => ids.has(e.source) && ids.has(e.target),
      );
      t.nodes.push(
        ...clipboard.map((n) => ({
          ...copy(n),
          id: ids.get(n.id)!,
          cells: n.cells?.map((c) => ({ ...copy(c), id: crypto.randomUUID() })),
          name: n.name + " copy",
          group: null,
          position: { x: n.position.x + 60, y: n.position.y + 60 },
        })),
      );
      t.links.push(
        ...links.map((e) => ({
          ...e,
          id: crypto.randomUUID(),
          source: ids.get(e.source)!,
          target: ids.get(e.target)!,
        })),
      );
    });
    get().setSelected([...ids.values()]);
  },
  duplicate: () => {
    get().copy();
    get().paste();
  },
  group: () => {
    const selected = get().topology.nodes.filter(
      (n) =>
        get().selected.includes(n.id) && n.kind !== "namespace" && !n.group,
    );
    if (!selected.length) return;
    const id = crypto.randomUUID();
    const x = Math.min(...selected.map((n) => n.position.x)) - 40,
      y = Math.min(...selected.map((n) => n.position.y)) - 65;
    get().edit((t) => {
      t.nodes.unshift({
        id,
        kind: "namespace",
        name: "Laboratory group",
        position: { x, y },
        config: defaults("namespace"),
      });
      for (const n of t.nodes) {
        if (selected.some((s) => s.id === n.id)) {
          n.group = id;
          n.position = { x: n.position.x - x, y: n.position.y - y };
        }
      }
    });
    get().setSelected([id]);
  },
}));
export async function api<T>(
  path: string,
  method = "GET",
  body?: unknown,
): Promise<T> {
  const res = await fetch("/api" + path, {
    method,
    credentials: "same-origin",
    headers: body === undefined ? {} : { "Content-Type": "application/json" },
    body: body === undefined ? undefined : JSON.stringify(body),
  });
  const data = await res.json();
  if (!res.ok) {
    const d = data.detail ?? data;
    throw new Error(typeof d === "string" ? d : JSON.stringify(d, null, 2));
  }
  return data;
}
export function downloadJSON(data: unknown, name: string) {
  const url = URL.createObjectURL(
    new Blob([JSON.stringify(data, null, 2)], { type: "application/json" }),
  );
  const a = document.createElement("a");
  a.href = url;
  a.download = name;
  a.click();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}
