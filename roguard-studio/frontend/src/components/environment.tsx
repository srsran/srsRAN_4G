"use client";
import { useEffect, useState } from "react";
import { api, useStudio, type Status, type MobilityPlan } from "@/lib/studio";
import { Button } from "./ui/button";

export function CapabilityMatrix() {
  const [report, setReport] = useState<{
    features: { name: string; class: string; studio: string }[];
    executable_limits: Record<string, number>;
  }>();
  const [error, setError] = useState("");
  useEffect(() => {
    void api<typeof report>("/capabilities")
      .then(setReport)
      .catch((e) => setError(String(e)));
  }, []);
  return (
    <div className="page-content">
      <div className="page-title">
        <div>
          <span>NATIVE AUDIT / EXECUTION BOUNDARY</span>
          <h1>Capability matrix</h1>
          <p>
            Native parser support, integrated execution and design visualization
            are tracked separately.
          </p>
        </div>
      </div>
      {error && <p role="alert">{error}</p>}
      <table>
        <thead>
          <tr>
            <th>Capability</th>
            <th>Class</th>
            <th>Studio status</th>
          </tr>
        </thead>
        <tbody>
          {report?.features.map((f) => (
            <tr key={f.name}>
              <td>{f.name}</td>
              <td>{f.class}</td>
              <td>{f.studio}</td>
            </tr>
          ))}
        </tbody>
      </table>
      <section className="surface">
        <h3>Current execution adapter</h3>
        <p className="help-text">
          One EPC and UE with up to three co-channel cells across one to three
          eNodeBs. The native FC32 broker combines downlink samples and fans out
          the uplink. Broker mode currently requires 50 PRBs, one EARFCN and
          unique PCIs. Handover and RF-driven mobility remain unavailable.
        </p>
        <p className="help-text">
          A: native and integrated · B: native, integration pending · C:
          requires I/Q/channel processing · D: unavailable through current
          engine adapter · E: visualization only.
        </p>
      </section>
    </div>
  );
}

export function Environment({
  status,
  mobility = false,
}: {
  status?: Status;
  mobility?: boolean;
}) {
  const { topology, edit } = useStudio();
  const radios = topology.nodes.filter((n) => ["enb", "ue"].includes(n.kind));
  const ues = radios.filter((n) => n.kind === "ue");
  const [selected, setSelected] = useState("");
  const ue = ues.find((u) => u.id === selected) ?? ues[0];
  const plan = topology.mobility?.find((p) => p.ue_id === ue?.id);
  const current: MobilityPlan = plan ?? {
    ue_id: ue?.id ?? "",
    start: { x: 0, y: 0 },
    end: { x: 100, y: 100 },
    speed: 1,
    duration: 60,
    mode: "planned",
  };
  const update = (value: Partial<MobilityPlan>) => {
    if (!ue) return;
    edit((t) => {
      t.mobility ??= [];
      const i = t.mobility.findIndex((p) => p.ue_id === ue.id);
      const next = { ...current, ...value };
      if (i < 0) t.mobility.push(next);
      else t.mobility[i] = next;
    });
  };
  const points = [
    ...radios.map((n) => n.position),
    ...(mobility && ue ? [current.start, current.end] : []),
  ];
  const minX = Math.min(0, ...points.map((p) => p.x)) - 100,
    minY = Math.min(0, ...points.map((p) => p.y)) - 100;
  const width = Math.max(1000, ...points.map((p) => p.x + 330)) - minX,
    height = Math.max(500, ...points.map((p) => p.y + 170)) - minY;
  return (
    <div className="page-content">
      <div className="page-title">
        <div>
          <span>
            {mobility ? "PLANNED EXPERIMENT DESIGN" : "2D RADIO ENVIRONMENT"}
          </span>
          <h1>{mobility ? "Mobility experiments" : "Radio environment"}</h1>
          <p>
            Design coordinates have no geographic calibration or physical
            propagation model.
          </p>
        </div>
      </div>
      <div className="config-notice">
        {mobility
          ? "PLANNED ONLY — saved path, speed and duration do not drive the radio. No motion or scheduled channel adapter is integrated."
          : "Design locations and configured transport are shown separately from native observations. No measured coverage heatmap is available."}
      </div>
      <svg
        className="environment-map"
        viewBox={`${minX} ${minY} ${width} ${height}`}
        aria-label="Designed radio environment"
      >
        <defs>
          <pattern
            id="env-grid"
            width="40"
            height="40"
            patternUnits="userSpaceOnUse"
          >
            <path d="M40 0H0V40" fill="none" stroke="#213744" strokeWidth="1" />
          </pattern>
        </defs>
        <rect
          x={minX}
          y={minY}
          width={width}
          height={height}
          fill="url(#env-grid)"
        />
        {topology.links
          .filter((l) => l.kind === "zmq")
          .map((l) => {
            const a = radios.find((n) => n.id === l.source),
              b = radios.find((n) => n.id === l.target);
            return a && b ? (
              <g key={l.id}>
                <line
                  x1={a.position.x}
                  y1={a.position.y}
                  x2={b.position.x}
                  y2={b.position.y}
                  stroke="#488caa"
                  strokeDasharray="7 6"
                />
                <text
                  className="map-note"
                  x={(a.position.x + b.position.x) / 2}
                  y={(a.position.y + b.position.y) / 2 - 10}
                >
                  Configured I/Q transport
                </text>
              </g>
            ) : null;
          })}
        {mobility && ue && (
          <g>
            <line
              x1={current.start.x}
              y1={current.start.y}
              x2={current.end.x}
              y2={current.end.y}
              stroke="#e4b979"
              strokeDasharray="4 7"
            />
            <circle
              cx={current.start.x}
              cy={current.start.y}
              r="5"
              fill="#e4b979"
            />
            <circle
              cx={current.end.x}
              cy={current.end.y}
              r="5"
              fill="#e4b979"
            />
            <text x={current.start.x + 8} y={current.start.y - 12}>
              Planned path · no RF effect
            </text>
          </g>
        )}
        {radios.map((n) => (
          <g
            key={n.id}
            transform={`translate(${n.position.x},${n.position.y})`}
          >
            <circle
              r={n.kind === "ue" ? 10 : 16}
              fill={n.kind === "ue" ? "#f2bc71" : "#6fbfd5"}
            />
            <text x="24" y="5">
              {n.name}
            </text>
            {n.cells?.map((c, i) => (
              <text className="map-note" x="24" y={26 + i * 18} key={c.id}>
                {c.name} · PCI {c.config.pci} / {c.config.dl_earfcn}{" "}
                (configured)
              </text>
            ))}
          </g>
        ))}
      </svg>
      <section className="surface">
        <h3>Native serving-cell evidence</h3>
        {status?.attached && status.serving_cell ? (
          <>
            <p>
              {status.serving_cell.name} · PCI {status.serving_cell.pci} ·
              EARFCN {status.serving_cell.earfcn}
            </p>
            <p className="help-text">
              {status.serving_cell.provenance}. Evidence belongs to applied run{" "}
              {status.run?.id}.
            </p>
          </>
        ) : (
          <p className="help-text">
            No current resolved serving cell. Configured transport and icon
            positions are not attachment evidence.
          </p>
        )}
        <p className="help-text">
          Handover: not verified. A change of reported PCI alone is not proof of
          a completed handover.
        </p>
      </section>
      {mobility && (
        <section className="surface">
          <h3>Path plan · design only</h3>
          {ue ? (
            <>
              <label className="field">
                <span>UE experiment target</span>
                <select
                  value={ue.id}
                  onChange={(e) => setSelected(e.target.value)}
                >
                  {ues.map((u) => (
                    <option key={u.id} value={u.id}>
                      {u.name}
                    </option>
                  ))}
                </select>
              </label>
              <div className="field-grid">
                {(["start", "end"] as const).flatMap((key) =>
                  (["x", "y"] as const).map((axis) => (
                    <label className="field" key={key + axis}>
                      <span>
                        {key} {axis} · design units
                      </span>
                      <input
                        type="number"
                        min={-100000}
                        max={100000}
                        value={current[key][axis]}
                        onChange={(e) =>
                          update({
                            [key]: {
                              ...current[key],
                              [axis]: Number(e.target.value),
                            },
                          })
                        }
                      />
                    </label>
                  )),
                )}
                <label className="field">
                  <span>Planned speed · design units/s</span>
                  <input
                    type="number"
                    min="0"
                    max="100"
                    value={current.speed}
                    onChange={(e) => update({ speed: Number(e.target.value) })}
                  />
                </label>
                <label className="field">
                  <span>Planned timeline · s</span>
                  <input
                    type="number"
                    min="1"
                    max="86400"
                    value={current.duration}
                    onChange={(e) =>
                      update({ duration: Number(e.target.value) })
                    }
                  />
                </label>
              </div>
              <Button onClick={() => update({})}>
                Add / update plan in design
              </Button>
              <p className="help-text">
                Save project to persist the plan. Speed and duration are
                independent planning constraints; no interpolation, attenuation
                or RF changes run.
              </p>
            </>
          ) : (
            <p>Add a UE in Network Builder first.</p>
          )}
        </section>
      )}
    </div>
  );
}
