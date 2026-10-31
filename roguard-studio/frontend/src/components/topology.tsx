"use client";
import { useCallback, useEffect, useMemo, useState } from "react";
import {
  ReactFlow,
  Background,
  BackgroundVariant,
  Controls,
  MiniMap,
  Handle,
  Position,
  useReactFlow,
  useNodesInitialized,
  type NodeProps,
  type Node,
  type Connection,
  type Edge,
  MarkerType,
  SelectionMode,
} from "@xyflow/react";
import {
  RadioTower,
  Server,
  Smartphone,
  ShieldAlert,
  Radio,
  Layers,
  Waves,
  Zap,
  Cable,
  Copy,
  Trash2,
  Group,
  Maximize2,
  LayoutGrid,
  Plus,
  MousePointer2,
  ScanLine,
} from "lucide-react";
import { Button } from "./ui/button";
import {
  useStudio,
  labels,
  type Kind,
  type Config,
  type Cell,
  type Status,
} from "@/lib/studio";
export const nodeIcons = {
  epc: Server,
  enb: RadioTower,
  ue: Smartphone,
  suspicious: ShieldAlert,
  temporary: Radio,
  namespace: Layers,
  channel: Waves,
  event: Zap,
  sector: Radio,
  zmq: Cable,
};
type Data = {
  name: string;
  kind: Kind;
  config: Config;
  state: string;
  applied: boolean;
  pending: boolean;
  members?: number;
  id: string;
  cells?: Cell[];
  serving?: string;
};
type RFNode = Node<Data>;
function StudioNode({ data, selected }: NodeProps<RFNode>) {
  const Icon = nodeIcons[data.kind];
  const [expanded, setExpanded] = useState(true);
  const healthy = data.state === "ready" && !data.pending;
  if (data.kind === "namespace")
    return (
      <div className={`namespace-node ${selected ? "selected" : ""}`}>
        <Layers size={14} />
        <span>{data.name}</span>
        <small>DESIGN GROUP</small>
      </div>
    );
  return (
    <div
      className={`network-node kind-${data.kind} ${selected ? "selected" : ""}`}
    >
      <Handle type="target" position={Position.Left} />
      <div className="node-eyebrow">
        <span>
          {data.kind === "epc"
            ? "CORE NETWORK"
            : data.kind === "ue"
              ? "USER EQUIPMENT"
              : ["suspicious", "temporary", "enb"].includes(data.kind)
                ? "RADIO ACCESS"
                : "SCENARIO OBJECT"}
        </span>
        <span
          className={`state-dot ${healthy ? "green" : data.state === "failed" ? "red" : ""}`}
        />
      </div>
      <div className="node-main">
        <span className="node-icon">
          <Icon size={25} strokeWidth={1.4} />
        </span>
        <div>
          <strong>{data.name}</strong>
          <small>{labels[data.kind]}</small>
        </div>
      </div>
      <div className="node-specs">
        {data.kind === "epc" ? (
          <>
            <span>
              PLMN{" "}
              <b>
                {data.config.mcc} / {data.config.mnc}
              </b>
            </span>
            <span>
              MME <b>{data.config.mme_addr}</b>
            </span>
          </>
        ) : data.kind === "ue" ? (
          <>
            <span>
              IMSI{" "}
              <b>
                {data.config.imsi.slice(0, 5)} ··· {data.config.imsi.slice(-4)}
              </b>
            </span>
            <span>
              Serving <b>{data.serving ?? "Not observed"}</b>
            </span>
          </>
        ) : (
          <>
            <span>
              Process ID <b>{data.config.enb_id}</b>
            </span>
            <span>
              Cells <b>{data.cells?.length ?? 0}</b>
            </span>
          </>
        )}
      </div>
      {data.kind === "enb" && (
        <div className="node-cells nodrag nowheel">
          <button
            className="cells-toggle"
            onClick={() => setExpanded(!expanded)}
            aria-expanded={expanded}
          >
            {expanded ? "▾" : "▸"} Owned cells ({data.cells?.length ?? 0})
          </button>
          {expanded &&
            data.cells?.map((cell) => (
              <button
                className="owned-cell"
                key={cell.id}
                onClick={(e) => {
                  e.stopPropagation();
                  useStudio.getState().setSelected([cell.id]);
                }}
              >
                <Radio size={14} />
                <span>
                  {cell.name}
                  <small>
                    PCI {cell.config.pci} · EARFCN {cell.config.dl_earfcn} · RF{" "}
                    {cell.config.rf_port}
                  </small>
                </span>
              </button>
            ))}
          {expanded && (
            <button
              className="add-cell"
              disabled={(data.cells?.length ?? 0) >= 5}
              onClick={(e) => {
                e.stopPropagation();
                useStudio.getState().addCell(data.id);
              }}
            >
              <Plus size={12} /> Add Cell
            </button>
          )}
        </div>
      )}
      <div className={`node-footer ${healthy ? "healthy" : ""}`}>
        <span>
          <i
            className={`state-dot ${healthy ? "green" : data.state === "failed" ? "red" : ""}`}
          />
          {data.pending
            ? "Pending · restart required"
            : data.state === "designed"
              ? "Designed · not applied"
              : data.state.replaceAll("-", " ")}
        </span>
        <span>
          {data.pending ? "DESIGN CHANGE" : data.applied ? "RUNTIME" : "DESIGN"}
        </span>
      </div>
      <Handle type="source" position={Position.Right} />
    </div>
  );
}
const nodeTypes = { studio: StudioNode };
export function TopologyCanvas({
  status,
  notify,
  layer,
}: {
  status: Status | undefined;
  notify: (s: string) => void;
  layer: string;
}) {
  const topology = useStudio((s) => s.topology),
    selected = useStudio((s) => s.selected),
    edit = useStudio((s) => s.edit),
    setSelected = useStudio((s) => s.setSelected);
  const flow = useReactFlow();
  const initialized = useNodesInitialized();
  useEffect(() => {
    if (initialized) void flow.fitView({ padding: 0.3, duration: 250 });
  }, [initialized, topology.id, layer, flow.fitView]);
  const [menu, setMenu] = useState<{ x: number; y: number } | null>(null);
  const [library, setLibrary] = useState(true);
  const designNodes = useMemo(() => {
    const ns =
      layer === "Applied"
        ? (status?.run?.topology.nodes ?? [])
        : topology.nodes;
    return ns.map((n) => {
      const role = ["temporary", "suspicious"].includes(n.kind)
        ? "enb"
        : n.kind;
      const applied = !!status?.run?.topology.nodes.some((a) => a.id === n.id);
      const members = ns.filter((c) => c.group === n.id);
      return {
        id: n.id,
        type: "studio",
        position: n.position,
        parentId: n.group ?? undefined,
        selected:
          selected.includes(n.id) ||
          !!n.cells?.some((c) => selected.includes(c.id)),
        data: {
          id: n.id,
          cells: n.cells,
          serving:
            n.kind === "ue" &&
            status?.run?.topology.nodes.some((a) => a.id === n.id)
              ? status?.serving_cell?.name
              : undefined,
          name: n.name,
          kind: n.kind,
          config: n.config,
          state: applied
            ? ((status?.node_components?.[n.id] ?? status?.components[role])
                ?.state ?? "stopped")
            : "designed",
          applied,
          pending:
            (applied &&
              layer === "Designed" &&
              JSON.stringify(
                status?.run?.topology.nodes.find((a) => a.id === n.id)?.config,
              ) !== JSON.stringify(n.config)) ||
            (applied &&
              layer === "Designed" &&
              JSON.stringify(
                status?.run?.topology.nodes.find((a) => a.id === n.id)?.cells,
              ) !== JSON.stringify(n.cells)),
          members: members.length,
        },
        style:
          n.kind === "namespace"
            ? {
                width: Math.max(420, ...members.map((m) => m.position.x + 330)),
                height: Math.max(
                  290,
                  ...members.map((m) => m.position.y + 245),
                ),
              }
            : undefined,
      } as RFNode;
    });
  }, [topology.nodes, status, selected, layer]);
  const edges: Edge[] = useMemo(
    () =>
      (layer === "Applied"
        ? (status?.run?.topology.links ?? [])
        : topology.links
      ).map((l) => ({
        id: l.id,
        source: l.source,
        target: l.target,
        type: "smoothstep",
        animated: false,
        label:
          l.kind === "s1"
            ? "S1 · CONTROL + USER PLANE"
            : l.kind === "zmq"
              ? "RF TRANSPORT · CONFIGURED I/Q"
              : "ANNOTATION",
        style: {
          stroke: l.kind === "s1" ? "#52717c" : "#3c93a5",
          strokeWidth: 1.5,
          strokeDasharray: l.kind === "zmq" ? "6 5" : undefined,
        },
        labelStyle: {
          fill: "#a8b6b8",
          fontSize: 9,
          fontFamily: "JetBrains Mono",
        },
        labelBgStyle: { fill: "#191c1e", fillOpacity: 1 },
        labelBgPadding: [9, 6],
        labelBgBorderRadius: 4,
        markerEnd: { type: MarkerType.ArrowClosed, color: "#547e87" },
      })),
    [topology.links, status, layer],
  );
  const isValidConnection = useCallback(
    (c: Connection | Edge) => {
      const a = topology.nodes.find((n) => n.id === c.source),
        b = topology.nodes.find((n) => n.id === c.target);
      if (
        !a ||
        !b ||
        a.id === b.id ||
        topology.links.some(
          (l) =>
            [l.source, l.target].includes(a.id) &&
            [l.source, l.target].includes(b.id),
        )
      )
        return false;
      const kinds = [a.kind, b.kind];
      return (
        kinds.some((k) => ["enb", "temporary", "suspicious"].includes(k)) &&
        (kinds.includes("ue") || kinds.includes("epc"))
      );
    },
    [topology],
  );
  const connect = useCallback(
    (c: Connection) => {
      if (!isValidConnection(c)) return;
      const a = topology.nodes.find((n) => n.id === c.source)!;
      const b = topology.nodes.find((n) => n.id === c.target)!;
      edit((t) =>
        t.links.push({
          id: crypto.randomUUID(),
          source: a.id,
          target: b.id,
          kind: a.kind === "epc" || b.kind === "epc" ? "s1" : "zmq",
        }),
      );
      notify("Designed connection added. Save and restart to apply.");
    },
    [topology, edit, isValidConnection, notify],
  );
  const autoLayout = () => {
    setLibrary(false);
    edit((t) => {
      let i = 0;
      for (const n of t.nodes) {
        n.group = null;
        n.position = {
          x: 80 + (i % 3) * 400,
          y: 150 + Math.floor(i / 3) * 510,
        };
        i++;
      }
    });
    setTimeout(() => flow.fitView({ padding: 0.3, duration: 400 }), 50);
  };
  return (
    <div className="canvas-workspace" onContextMenu={(e) => e.preventDefault()}>
      <div className="canvas-topline">
        <div className="breadcrumb">
          WORKSPACE <span>/</span> {topology.name} <span>/</span>{" "}
          <b>{layer.toUpperCase()} TOPOLOGY</b>
        </div>
        <span className="mono dim">
          {topology.nodes.length} components · {topology.links.length} links
        </span>
      </div>
      <div className="canvas-tools">
        <div className="tool-group">
          <Button
            size="icon"
            title="Select and pan"
            onClick={() =>
              notify(
                "Drag canvas to pan. Shift + drag to select multiple components.",
              )
            }
          >
            <MousePointer2 size={15} />
          </Button>
          <Button
            size="icon"
            title="Fit topology"
            onClick={() => flow.fitView({ padding: 0.3, duration: 400 })}
          >
            <Maximize2 size={15} />
          </Button>
          <Button
            size="icon"
            title="Automatic layout"
            onClick={autoLayout}
            disabled={layer !== "Designed"}
          >
            <LayoutGrid size={15} />
          </Button>
          <Button
            size="icon"
            title="Group selected components"
            onClick={() => useStudio.getState().group()}
            disabled={layer !== "Designed"}
          >
            <Group size={15} />
          </Button>
        </div>
        <Button onClick={() => setLibrary(!library)}>
          <Plus size={14} /> Component library
        </Button>
      </div>
      {layer === "Observed" ? (
        <div className="observed-canvas">
          <ScanLine size={42} />
          <h2>Observed network</h2>
          {status?.observed ? (
            <>
              <p>
                Measured PCI <b>{status.observed.parameters.pci}</b> · EARFCN{" "}
                <b>{status.observed.parameters.earfcn}</b>
              </p>
              <span>{status.observed.source}</span>
              <small>
                Last observed {status.observed.timestamp}.{" "}
                {status.attached
                  ? "UE attachment evidenced."
                  : "No current attached UE."}{" "}
                {status.serving_cell
                  ? `Serving cell: ${status.serving_cell.name}. ${status.serving_cell.provenance}.`
                  : "Serving cell identity unresolved; no current unique applied match."}{" "}
                PLMN, TAC and handover are not inferred from the design.
              </small>
            </>
          ) : (
            <>
              <p>No radio observation has been collected.</p>
              <small>
                Start a valid LTE network. An attached UE metrics row supplies
                PCI and EARFCN.
              </small>
            </>
          )}
        </div>
      ) : (
        <ReactFlow<RFNode>
          nodes={designNodes}
          edges={edges}
          nodeTypes={nodeTypes}
          onConnect={connect}
          isValidConnection={isValidConnection}
          nodesDraggable={layer === "Designed"}
          nodesConnectable={layer === "Designed"}
          onNodesChange={(changes) => {
            const selections = changes.filter((c) => c.type === "select");
            if (selections.length) {
              const ids = new Set(useStudio.getState().selected);
              for (const c of selections)
                if (c.type === "select") {
                  if (c.selected) ids.add(c.id);
                  else ids.delete(c.id);
                }
              setSelected([...ids]);
            }
            const removed = changes
              .filter((c) => c.type === "remove")
              .map((c) => c.id);
            if (removed.length && layer === "Designed") {
              setSelected(removed);
              useStudio.getState().remove();
            }
            const moved = changes.filter((c) => {
              if (c.type !== "position" || !c.position) return false;
              const n = useStudio
                .getState()
                .topology.nodes.find((n) => n.id === c.id);
              return (
                n &&
                (n.position.x !== c.position.x || n.position.y !== c.position.y)
              );
            });
            if (moved.length && layer === "Designed")
              edit((t) => {
                for (const c of moved)
                  if (c.type === "position" && c.position) {
                    const n = t.nodes.find((n) => n.id === c.id);
                    if (n) n.position = c.position;
                  }
              }, false);
          }}
          onEdgesChange={(changes) => {
            const ids = changes
              .filter((c) => c.type === "remove")
              .map((c) => c.id);
            if (ids.length && layer === "Designed")
              edit((t) => {
                t.links = t.links.filter((l) => !ids.includes(l.id));
              });
          }}
          onNodeDragStart={() => edit(() => {})}
          onNodeContextMenu={(e, n) => {
            e.preventDefault();
            setSelected([n.id]);
            setMenu({ x: e.clientX, y: e.clientY });
          }}
          onPaneClick={() => setMenu(null)}
          onDrop={(e) => {
            e.preventDefault();
            if (layer !== "Designed") return;
            const kind = e.dataTransfer.getData("application/roguard") as Kind;
            if (kind in labels)
              useStudio
                .getState()
                .add(
                  kind,
                  flow.screenToFlowPosition({ x: e.clientX, y: e.clientY }),
                );
          }}
          onDragOver={(e) => {
            e.preventDefault();
            e.dataTransfer.dropEffect = "move";
          }}
          fitView
          fitViewOptions={{ padding: 0.32 }}
          minZoom={0.25}
          maxZoom={1.8}
          snapToGrid
          snapGrid={[20, 20]}
          deleteKeyCode={null}
          selectionOnDrag
          selectionMode={SelectionMode.Partial}
          panOnDrag={[1, 2]}
          multiSelectionKeyCode="Shift"
          colorMode="dark"
          proOptions={{ hideAttribution: false }}
        >
          <Background
            variant={BackgroundVariant.Dots}
            gap={24}
            size={1}
            color="#363a3d"
          />
          <Controls showInteractive={false} />
          <MiniMap
            nodeColor={(n) =>
              n.data.kind === "suspicious" ? "#ac6464" : "#417e8b"
            }
            maskColor="rgba(19,22,24,.8)"
            pannable
            zoomable
          />
        </ReactFlow>
      )}
      {layer === "Applied" && !status?.run && (
        <div className="canvas-empty">
          <RadioTower size={34} />
          <h3>No applied runtime topology</h3>
          <p>Saved designs appear here after a real run starts.</p>
        </div>
      )}
      {library && layer === "Designed" && (
        <div className="component-library">
          <header>
            <span>NETWORK PROCESSES</span>
            <button
              onClick={() => setLibrary(false)}
              aria-label="Close component library"
            >
              ×
            </button>
          </header>
          <p>Add a process; configure cells inside an eNodeB</p>
          <div className="library-items">
            {(["epc", "enb", "ue"] as Kind[]).map((k) => {
              const Icon = nodeIcons[k];
              return (
                <button
                  key={k}
                  draggable
                  onDragStart={(e) =>
                    e.dataTransfer.setData("application/roguard", k)
                  }
                  onClick={() => useStudio.getState().add(k)}
                  title={labels[k]}
                >
                  <Icon size={17} />
                  <span>{labels[k]}</span>
                  <Plus size={12} />
                </button>
              );
            })}
          </div>
          <footer>
            <i className="state-dot" /> ZeroMQ · direct / co-channel broker
          </footer>
        </div>
      )}
      <div className="canvas-legend">
        <span>
          <i className="state-dot cyan" />
          Infrastructure
        </span>
        <span>
          <i className="state-dot green" />
          Runtime ready
        </span>
        <span>
          <i className="state-dot amber" />
          Pending change
        </span>
        <span>
          <i className="state-dot red" />
          Failed · see runtime evidence
        </span>
      </div>
      <div className="canvas-note">
        <span className="mono">
          {layer === "Designed"
            ? "DESIGN MODE"
            : layer === "Applied"
              ? "APPLIED SNAPSHOT"
              : "MEASURED EVIDENCE"}
        </span>
        <p>
          {layer === "Designed"
            ? "Canvas edits do not change a running radio."
            : layer === "Applied"
              ? "Configuration captured when the real run started."
              : "Only verified measurements appear in this layer."}
        </p>
      </div>
      {menu && (
        <div className="context-menu" style={{ left: menu.x, top: menu.y }}>
          <button
            onClick={() => {
              useStudio.getState().duplicate();
              setMenu(null);
            }}
          >
            <Copy size={14} />
            Duplicate
          </button>
          <button
            onClick={() => {
              useStudio.getState().group();
              setMenu(null);
            }}
          >
            <Group size={14} />
            Group selection
          </button>
          <button
            onClick={() => {
              useStudio.getState().remove();
              setMenu(null);
            }}
          >
            <Trash2 size={14} />
            Delete from design
          </button>
        </div>
      )}
    </div>
  );
}
