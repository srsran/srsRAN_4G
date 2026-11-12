"use client";

import { useEffect, useState } from "react";
import { useQuery, useQueryClient } from "@tanstack/react-query";
import { Loader2, PlugZap } from "lucide-react";
import { api } from "@/lib/studio";
import { Button } from "./ui/button";

type Connection = {
  dashboard_url: string;
  studio_url: string;
  connected: boolean;
  state: string;
};

export function RoguardConnection({
  onConnected,
  onCancel,
  actionLabel = "Connect RoGuard",
}: {
  onConnected?: () => Promise<void>;
  onCancel?: () => void;
  actionLabel?: string;
}) {
  const qc = useQueryClient();
  const [dashboardUrl, setDashboardUrl] = useState("");
  const [studioUrl, setStudioUrl] = useState("");
  const [loaded, setLoaded] = useState(false);
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState("");
  const connection = useQuery({
    queryKey: ["roguard-connection"],
    queryFn: () => api<Connection>("/roguard/connection"),
    refetchInterval: 4000,
  });
  useEffect(() => {
    if (connection.data && !loaded) {
      setDashboardUrl(connection.data.dashboard_url);
      setStudioUrl(connection.data.studio_url);
      setLoaded(true);
    }
  }, [connection.data, loaded]);

  return (
    <form onSubmit={async (event) => {
      event.preventDefault();
      setBusy(true);
      setError("");
      try {
        const saved = await api<Connection>("/roguard/connection", "POST", {
          dashboard_url: dashboardUrl,
          studio_url: studioUrl,
        });
        qc.setQueryData(["roguard-connection"], saved);
        setDashboardUrl(saved.dashboard_url);
        setStudioUrl(saved.studio_url);
        await onConnected?.();
      } catch (reason) {
        setError(String(reason));
      } finally {
        setBusy(false);
      }
    }}>
      <h3>RoGuard connection</h3>
      <p>Connect RoGuard to this simulation using your existing local credentials.</p>
      <label className="field">
        <span>RoGuard dashboard URL</span>
        <input required disabled={busy || !loaded} value={dashboardUrl}
          placeholder="http://localhost:3000"
          onChange={(event) => setDashboardUrl(event.target.value)} />
      </label>
      <label className="field">
        <span>Studio API URL</span>
        <input required disabled={busy || !loaded} value={studioUrl}
          placeholder="http://127.0.0.1:8000"
          onChange={(event) => setStudioUrl(event.target.value)} />
      </label>
      <p className="help-text">Use the RoGuard dashboard’s port and Studio’s control API port. Both services must be running. Settings are saved and the bridge reconnects automatically.</p>
      <p role="status">
        {busy ? "Connecting RoGuard and waiting for live ingestion…" :
          connection.data?.connected ? "RoGuard connected to this Studio" :
          `RoGuard: ${connection.data?.state ?? "loading"}`}
      </p>
      {(error || connection.error) && <p role="alert">{error || String(connection.error)}</p>}
      <div className="modal-actions">
        {onCancel && <Button type="button" disabled={busy} onClick={onCancel}>Cancel</Button>}
        <Button type="submit" variant="default" disabled={busy || !loaded}>
          {busy ? <Loader2 size={14} className="spin" /> : <PlugZap size={14} />}
          {busy ? "Connecting…" : actionLabel}
        </Button>
      </div>
    </form>
  );
}
