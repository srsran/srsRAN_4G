"use client";
import { useEffect, useState } from "react";
import { useQuery, useQueryClient } from "@tanstack/react-query";
import { api, useStudio, type Topology, type Status, type Validation, type RFPlan } from "@/lib/studio";
import { Button } from "./ui/button";

type Template = {id:string;version:number;name:string;purpose:string;topology:Topology;required_adapter:string;expected_behavior:string;limitations:string[];label:string;compatibility:NonNullable<Validation["compatibility"]>};

export function CompatibilityPanel() {
  const {topology, edit}=useStudio();
  const [design,setDesign]=useState(topology);
  useEffect(()=>{const timer=setTimeout(()=>setDesign(topology),250);return()=>clearTimeout(timer);},[topology]);
  const check=useQuery({queryKey:["compatibility",design],queryFn:()=>api<Validation>("/compatibility","POST",design)});
  const report=check.data;
  return <section className="surface" aria-label="Configuration compatibility">
    <h3>Configuration compatibility</h3>
    <label className="field"><span>Execution adapter</span><select value={topology.execution_adapter??"auto"} onChange={e=>edit(t=>{t.execution_adapter=e.target.value as Topology["execution_adapter"];})}>
      <option value="auto">Automatic selection</option><option value="legacy">Preserved direct / co-channel adapter</option><option value="matrix-iq-v2">Independent UE / frequency matrix</option>
    </select></label>
    {check.error&&<p role="alert">{String(check.error)}</p>}
    <p role="status">{report?.compatibility?.status??"Checking saved design parameters…"}</p>
    {report?.compatibility&&<p className="help-text">Adapter: {report.compatibility.adapter}. Native verification applies to the exact radio profile and software implementation. {report.compatibility.native_verified?"Matching native acceptance evidence is recorded.":"Code support does not certify native attachment for this combination."}</p>}
    {[...(report?.errors??[]),...(report?.adapter_blockers??[]),...(report?.runtime_blockers??[])].map((message,i)=><p role="alert" key={i}>{message}</p>)}
    {report?.compatibility?.radio_profiles?.map(profile=><p className="help-text" key={profile.cell_id}>Band 7 · EARFCN {profile.earfcn} · DL {profile.dl_mhz.toFixed(1)} MHz / UL {profile.ul_mhz.toFixed(1)} MHz · {profile.n_prb} PRBs · native {(profile.native_rate_hz/1e6).toFixed(2)} Msps / transport {(profile.transport_rate_hz/1e6).toFixed(2)} Msps</p>)}
    {report?.rf_plan&&<p className="help-text">{report.rf_plan.model}</p>}
  </section>;
}

export function NetworkTemplates({go}:{go:(page:string)=>void}) {
  const queryClient=useQueryClient();const [chosen,setChosen]=useState<Template>();const [error,setError]=useState("");const [busy,setBusy]=useState(false);
  const templates=useQuery({queryKey:["network-templates"],queryFn:()=>api<Template[]>("/network-templates")});
  return <div className="page-content"><div className="page-title"><div><span>VERSIONED LTE LABORATORIES</span><h1>Network templates</h1><p>Preview a supported design and create a new editable project. Existing projects and applied runs are preserved.</p></div></div>
    {(error||templates.error)&&<p role="alert">{error||String(templates.error)}</p>}
    <div className="experiment-grid">{templates.data?.map(template=><section className="surface" key={template.id}>
      <h3>{template.name}</h3><p>{template.label}</p><p className="help-text">Version {template.version} · {template.required_adapter}</p>
      <Button onClick={()=>setChosen(template)}>Preview {template.name}</Button>
    </section>)}</div>
    {chosen&&<section className="surface" aria-label="Template preview"><h3>{chosen.name}</h3><p>{chosen.label}</p><p>{chosen.purpose}</p><p>{chosen.expected_behavior}</p>
      <table><thead><tr><th>Component</th><th>Role</th><th>Owned cells</th></tr></thead><tbody>{chosen.topology.nodes.map(n=><tr key={n.id}><td>{n.name}</td><td>{n.kind}</td><td>{n.cells?.map(c=>`PCI ${c.config.pci} / EARFCN ${c.config.dl_earfcn}`).join(", ")||"—"}</td></tr>)}</tbody></table>
      <ul>{chosen.limitations.map(l=><li key={l}>{l}</li>)}</ul>
      <Button disabled={busy} onClick={async()=>{setBusy(true);setError("");try{const project=await api<Topology>(`/network-templates/${chosen.id}/projects`,"POST");useStudio.getState().load(project);go("Network Builder");await queryClient.invalidateQueries({queryKey:["topologies"]});}catch(reason){setError(String(reason));}finally{setBusy(false);}}}>Create editable project</Button>
    </section>}
  </div>;
}

export function IndependentUes({status}:{status?:Status}) {
  const {topology}=useStudio();const ues=Object.entries(status?.ues??{});
  if(!ues.length)return null;
  return <section className="surface" aria-label="Independent UE evidence"><h3>Independent UE evidence</h3><p className="help-text">NAS attachment comes from native NAS logs. RRC connection and serving parameters come from each UE's own CSV. Stopped or stale serving associations remain unresolved.</p>
    {ues.map(([id,u])=><div className="check-row" key={id} data-ue-id={id}><strong>{topology.nodes.find(n=>n.id===id)?.name??id}</strong><span>{u.state}</span><span>NAS: {u.nas_attached?"attached":"not attached"} · Last RRC sample: {u.rrc_connected?"connected":"idle/unavailable"}</span><span>{u.serving_cell?`PCI ${u.serving_cell.pci} · EARFCN ${u.serving_cell.earfcn}`:"No current resolved serving cell"}</span><span>Last assigned IPv4: {u.ip??"unavailable"}</span></div>)}
  </section>;
}


export function MatrixTransport({plan,status}:{plan:RFPlan;status?:Status}) {
  const active=!!status?.run&&["starting","running"].includes(status.state);
  const sockets=useQuery({enabled:active,queryKey:["matrix-native-sockets",status?.run?.id],queryFn:()=>api<{runtime?:{text:string}}>("/zeromq"),refetchInterval:3000});
  const environments=(status?.run?.environments??{}) as Record<string,string>;
  const brokers=(active?status?.run?.brokers??{}:{}) as Record<string,{data:unknown}>;
  return <div className="page-content"><div className="page-title"><div><span>INDEPENDENT NATIVE RF PATHS</span><h1>ZeroMQ matrix transport</h1><p>{plan.model}</p></div></div>
    <p className="config-notice">{active?"Applied allocation with current native evidence":"Designed allocation — no current native run for this project"}</p>
    {plan.channels?.map(channel=><section className="surface" key={channel.id}><h3>{channel.role} · EARFCN {channel.earfcn}</h3><p className="help-text">{environments[channel.core_id]==="stopped"?"Owned core stopped; its counters are historical":active?"Current owned environment":"Designed environment"}</p><p>Band 7 · {channel.n_prb} PRBs · {(channel.sample_rate/1e6).toFixed(2)} Msps FC32 · core {channel.core_id}</p>
      <table><thead><tr><th>Source</th><th>Native TX</th><th>Broker RX output</th></tr></thead><tbody>
        {plan.paths.filter(path=>path.environment_id===channel.id).map(path=><tr key={path.cell_id}><td>Cell {path.cell_id} / {path.role} RF {path.rf_port}</td><td>{path.native_tx}</td><td>{path.broker_ul}</td></tr>)}
        {plan.ues?.filter(ue=>ue.environment_id===channel.id).map(ue=><tr key={ue.component_id}><td>{ue.role} · TUN namespace {ue.tun_namespace}</td><td>{ue.ue_tx}</td><td>{ue.broker_dl}</td></tr>)}
      </tbody></table>
      <p className="help-text">Each UE supplies independent uplink samples. Counters confirm transport, not successful cell decoding. Separate EARFCNs have separate brokers.</p>
      <details><summary>Native broker counters · {channel.role}</summary><pre className="code-block">{brokers[channel.role]?JSON.stringify(brokers[channel.role].data,null,2):"No native broker sample yet"}</pre></details>
    </section>)}
    <section className="surface"><h3>Native socket evidence</h3><pre className="code-block">{active?sockets.data?.runtime?.text??"No native socket sample yet":"No current native socket sample for this project"}</pre></section>
  </div>;
}
