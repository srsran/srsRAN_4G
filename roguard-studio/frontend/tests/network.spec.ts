import {test,expect} from "@playwright/test";
import {nativeAction} from "./native-helpers";

test.afterEach(async({page})=>{await page.request.post('/api/simulations/stop',{headers:{Origin:'http://127.0.0.1:3000'}});});

for(const scenario of [
  {name:"Two independent UEs",ues:2,cells:1,frequencies:[3350,3350]},
  {name:"25 PRB co-channel experiment",ues:1,cells:2,frequencies:[3350]},
  {name:"Two fixed-frequency RF environments",ues:2,cells:2,frequencies:[3350,3400]},
  {name:"Four eNodeBs / four cells",ues:1,cells:4,frequencies:[3350]},
  {name:"Two independently isolated cores",ues:2,cells:2,frequencies:[3350,3350]},
]) {
  test(`native template: ${scenario.name}`,async({page})=>{
    test.setTimeout(220000);
    await page.goto('/');await expect(page.getByText('Engine connected',{exact:true})).toBeVisible();
    await page.getByRole('button',{name:'Network Templates',exact:true}).click();
    await page.getByRole('button',{name:`Preview ${scenario.name}`,exact:true}).click();
    await expect(page.getByLabel('Template preview')).toContainText('Executable — native verified');
    await page.getByRole('button',{name:'Create editable project',exact:true}).click();
    await expect(page.locator('.canvas-workspace')).toBeVisible();
    await page.getByRole('button',{name:'Simulation Runtime',exact:true}).click();
    await expect(page.getByLabel('Configuration compatibility')).toContainText('Supported and native-verified');
    await nativeAction(page);
    await expect(page.locator('.runtime-indicator')).toContainText('Simulation running',{timeout:160000});
    const evidence=page.getByLabel('Independent UE evidence');await expect(evidence.locator('[data-ue-id]')).toHaveCount(scenario.ues);
    await expect.poll(async()=>{
      const s=await (await page.request.get('/api/simulations')).json();
      return Object.values(s.ues).filter((u:any)=>u.nas_attached&&u.serving_cell).length;
    },{timeout:20000}).toBe(scenario.ues);
    const status=await (await page.request.get('/api/simulations')).json();
    const ues:any[]=Object.values(status.ues);
    expect(ues.map(u=>u.serving_cell.earfcn).sort()).toEqual([...scenario.frequencies].sort());
    expect(new Set(Object.values(status.node_components).map((p:any)=>p.pid)).size).toBe(status.run.topology.nodes.length);
    await page.getByRole('button',{name:'Native Measurements',exact:true}).click();
    await expect(page.getByLabel('UE measurement source').locator('option')).toHaveCount(scenario.ues,{timeout:15000});
    if(scenario.ues>1)await page.getByLabel('UE measurement source').selectOption(ues[1].component_id);
    await expect(page.locator('.stat-card').first().locator('strong')).not.toHaveText('—dBm');
    if(scenario.name==='Two independently isolated cores'){
      await page.getByRole('button',{name:'Simulation Runtime',exact:true}).click();
      const cores=status.run.rf_plan.cores;
      await page.getByRole('button',{name:'Stop owned core Core 2',exact:true}).click();
      await expect.poll(async()=>{
        const s=await (await page.request.get('/api/simulations')).json();
        return s.run.environments?.[cores[1].component_id];
      },{timeout:35000}).toBe('stopped');
      const s=await (await page.request.get('/api/simulations')).json();
      expect(Object.values(s.ues).some((u:any)=>u.core_id===cores[0].component_id&&u.nas_attached)).toBe(true);
    }
    await page.getByRole('button',{name:'ZeroMQ / RF Transport',exact:true}).click();
    await expect(page.getByRole('heading',{name:'ZeroMQ matrix transport',exact:true})).toBeVisible();
    if(scenario.name!=='Two independently isolated cores')await expect(page.getByText('Applied allocation with current native evidence',{exact:true})).toBeVisible();
    const t=status.run.topology;
    const captured=await page.request.post('/api/experiments',{headers:{Origin:'http://127.0.0.1:3000'},data:{name:'Native topology browser evidence',topology_id:t.id}});
    expect(captured.status()).toBe(201);
    const id=(await captured.json()).id;
    const exported=await (await page.request.get(`/api/experiments/${id}/export?format=json`)).json();
    expect(new Set(exported.metrics.filter((m:any)=>m.component==='ue').map((m:any)=>m.component_id)).size).toBe(scenario.ues);
    const restore=await page.request.post(`/api/experiments/${id}/restore`,{headers:{Origin:'http://127.0.0.1:3000'}});
    expect(restore.status()).toBe(200);const restored=await restore.json();
    expect(restored.id).not.toBe(t.id);expect((await page.request.get(`/api/topologies/${restored.id}`)).status()).toBe(200);
    expect((await (await page.request.get('/api/simulations')).json()).state).toBe('stopped');
    await page.screenshot({path:`test-results/network-${scenario.cells}cell-${scenario.name.includes('frequenc')?'frequencies':scenario.name.includes('cores')?'cores':scenario.ues+'ue'}.png`,fullPage:true});
  });
}

test('invalid multi-UE subscriber and unsupported core diagnostics are explicit',async({page})=>{
  await page.goto('/');await expect(page.getByText('Engine connected',{exact:true})).toBeVisible();
  const templates=await (await page.request.get('/api/network-templates')).json();
  const t=structuredClone(templates.find((x:any)=>x.id==='two-ues').topology);
  const ues=t.nodes.filter((n:any)=>n.kind==='ue');ues[1].config.imsi=ues[0].config.imsi;
  const result=await (await page.request.post('/api/compatibility',{headers:{Origin:'http://127.0.0.1:3000'},data:t})).json();
  expect(result.compatibility.status).toBe('Invalid configuration');expect(result.errors.join(' ')).toContain('unique synthetic IMSI');
  t.nodes.find((n:any)=>n.kind==='epc').core_adapter='open5gs';ues[1].config.imsi='001010123456789';
  const blocked=await (await page.request.post('/api/compatibility',{headers:{Origin:'http://127.0.0.1:3000'},data:t})).json();
  expect(blocked.runnable).toBe(false);expect(blocked.adapter_blockers.join(' ')).toContain('Open5GS');
});
