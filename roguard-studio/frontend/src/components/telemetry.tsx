"use client";
import { useEffect, useRef, useState } from "react";
import * as echarts from "echarts/core";
import { LineChart } from "echarts/charts";
import {
  GridComponent,
  TooltipComponent,
  LegendComponent,
} from "echarts/components";
import { CanvasRenderer } from "echarts/renderers";
import {
  Activity,
  Download,
  Radio,
  Signal,
  Gauge,
  ArrowDownUp,
} from "lucide-react";
import { type Metric } from "@/lib/studio";
import { Button } from "./ui/button";
echarts.use([
  LineChart,
  GridComponent,
  TooltipComponent,
  LegendComponent,
  CanvasRenderer,
]);
export function Telemetry({ metrics }: { metrics: Metric[] }) {
  const ref = useRef<HTMLDivElement>(null);
  const [windowMinutes, setWindowMinutes] = useState(5);
  const [metric, setMetric] = useState("rsrp");
  const [clock, setClock] = useState(() => Date.now());
  useEffect(() => {
    const id = setInterval(() => setClock(Date.now()), 5000);
    return () => clearInterval(id);
  }, []);
  const rows = metrics.filter(
    (m) =>
      m.component === "ue" &&
      new Date(m.timestamp).getTime() > clock - windowMinutes * 60000,
  );
  const last = rows.at(-1);
  const series = rows
    .filter((m) => m.values[metric] != null)
    .map((m) => [new Date(m.timestamp).getTime(), m.values[metric]]);
  const units: Record<string, string> = {
    rsrp: "dBm",
    dl_snr: "dB",
    dl_brate: "bit/s",
    ul_brate: "bit/s",
    dl_bler: "%",
    ul_bler: "%",
  };
  useEffect(() => {
    if (!ref.current) return;
    const chart = echarts.init(ref.current, undefined, { renderer: "canvas" });
    chart.setOption({
      backgroundColor: "transparent",
      animation: false,
      grid: { left: 70, right: 30, top: 30, bottom: 45 },
      tooltip: { trigger: "axis" },
      xAxis: {
        type: "time",
        axisLabel: {
          color: "#899195",
          fontFamily: "JetBrains Mono",
          fontSize: 10,
        },
        axisLine: { lineStyle: { color: "#34383a" } },
      },
      yAxis: {
        type: "value",
        name: units[metric],
        nameTextStyle: { color: "#899195" },
        axisLabel: {
          color: "#899195",
          fontFamily: "JetBrains Mono",
          fontSize: 10,
        },
        splitLine: { lineStyle: { color: "#2c3032" } },
      },
      series: [
        {
          name: metric,
          type: "line",
          data: series,
          showSymbol: false,
          lineStyle: { color: "#70c5d4", width: 2 },
          itemStyle: { color: "#70c5d4" },
          connectNulls: false,
        },
      ],
    });
    const observer = new ResizeObserver(() => chart.resize());
    observer.observe(ref.current);
    return () => {
      observer.disconnect();
      chart.dispose();
    };
  }, [metrics, metric, windowMinutes, clock]);
  const download = () => {
    const header = "timestamp_utc,run_id,component,source,metric,value\n";
    const body = metrics
      .flatMap((m) =>
        Object.entries(m.values).map(([k, v]) =>
          [m.timestamp, m.run_id, m.component, m.source, k, v ?? ""].join(","),
        ),
      )
      .join("\n");
    const url = URL.createObjectURL(
      new Blob([header + body], { type: "text/csv" }),
    );
    const a = document.createElement("a");
    a.href = url;
    a.download = "roguard-metrics.csv";
    a.click();
    URL.revokeObjectURL(url);
  };
  return (
    <div className="page-content">
      <div className="page-title">
        <div>
          <span className="eyebrow">ENGINEERING MEASUREMENTS</span>
          <h1>Live telemetry</h1>
          <p>
            Actual srsUE CSV samples. Unavailable measurements remain empty.
          </p>
        </div>
        <Button onClick={download} disabled={!metrics.length}>
          <Download size={14} /> Export CSV
        </Button>
      </div>
      <div className="stat-cards">
        {[
          { label: "RSRP", key: "rsrp", unit: "dBm", Icon: Signal },
          { label: "DOWNLINK SNR", key: "dl_snr", unit: "dB", Icon: Radio },
          {
            label: "DOWNLINK",
            key: "dl_brate",
            unit: "bit/s",
            Icon: ArrowDownUp,
          },
          { label: "DOWNLINK BLER", key: "dl_bler", unit: "%", Icon: Gauge },
        ].map(({ label, key, unit, Icon }) => (
          <div className="stat-card" key={key}>
            <span>
              {label}
              <Icon size={15} />
            </span>
            <strong>
              {last?.values[key] != null ? last.values[key]?.toFixed(1) : "—"}
              <small>{unit}</small>
            </strong>
            <p>
              {last
                ? "srsUE metrics CSV · " +
                  new Date(last.timestamp).toLocaleTimeString()
                : "Not available · awaiting engine output"}
            </p>
          </div>
        ))}
      </div>
      <div className="chart-panel">
        <div className="chart-toolbar">
          <span>
            <Activity size={15} /> UE radio measurements
          </span>
          <div>
            <select
              aria-label="Metric"
              value={metric}
              onChange={(e) => setMetric(e.target.value)}
            >
              {Object.keys(units).map((k) => (
                <option key={k} value={k}>
                  {k.toUpperCase()} · {units[k]}
                </option>
              ))}
            </select>
            <select
              aria-label="Time window"
              value={windowMinutes}
              onChange={(e) => setWindowMinutes(Number(e.target.value))}
            >
              <option value={1}>Last minute</option>
              <option value={5}>Last 5 minutes</option>
              <option value={30}>Last 30 minutes</option>
              <option value={1440}>Last 24 hours</option>
            </select>
          </div>
        </div>
        <div ref={ref} className="engineering-chart" />
        {!series.length && (
          <div className="chart-empty">
            <Activity size={28} />
            <h3>No measurements in this window</h3>
            <p>
              A chart appears when the engine emits actual {metric} samples.
            </p>
          </div>
        )}
      </div>
      <div className="info-strip">
        RSRQ and CQI are not present in the installed UE CSV schema. No
        substitute values are generated. Throughput is reported in bit/s; BLER
        is a block error percentage, not packet loss.
      </div>
    </div>
  );
}
