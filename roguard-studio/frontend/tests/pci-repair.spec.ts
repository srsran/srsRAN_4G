import { test, expect } from "@playwright/test";

test("remaining eNodeB allocates distinct PCIs and repairs a conflict before native start", async ({
  page,
}) => {
  test.setTimeout(150000);
  page.setDefaultTimeout(10000);
  const errors: string[] = [];
  page.on("pageerror", (error) => errors.push(error.message));
  await page.goto("/");
  await expect(
    page.getByText("Engine connected", { exact: true }),
  ).toBeVisible();
  await page.locator(".project-switcher>button").click();
  await page.getByRole("button", { name: "New project", exact: true }).click();
  await page
    .getByLabel("Project name", { exact: true })
    .fill("PCI repair native regression");
  await page
    .getByRole("button", { name: "Create project", exact: true })
    .click();
  for (const name of [
    "Evolved Packet Core",
    "LTE eNodeB",
    "User Equipment",
    "LTE eNodeB",
  ])
    await page
      .locator(".library-items button")
      .filter({ hasText: name })
      .click();
  await page.getByTitle("Automatic layout", { exact: true }).click();
  await page.locator(".kind-enb .node-main").first().click();
  await page.keyboard.press("Delete");
  await expect(page.locator(".kind-enb")).toHaveCount(1);
  await page.getByTitle("Automatic layout", { exact: true }).click();
  await page.locator(".kind-enb .owned-cell").click();
  await expect(
    page.getByLabel("Physical cell ID", { exact: true }),
  ).toHaveValue("6");
  await page
    .getByLabel("Broker path attenuation · dB")
    .fill("0");
  await page.locator(".kind-enb .add-cell").click();
  await expect(page.locator(".kind-enb .owned-cell")).toHaveCount(2);
  await page.locator(".kind-enb .owned-cell").nth(1).click();
  await expect(
    page.getByLabel("Physical cell ID", { exact: true }),
  ).toHaveValue("1");
  await page.getByRole("button", { name: "Save project", exact: true }).click();
  await expect(page.getByRole("status")).toContainText("Project saved");
  const projects = await (await page.request.get("/api/topologies")).json();
  const before = projects.find(
    (t: { name: string }) => t.name === "PCI repair native regression",
  );
  const enb = before.nodes.find((n: { kind: string }) => n.kind === "enb");
  const ue = before.nodes.find((n: { kind: string }) => n.kind === "ue");
  expect(enb.config.enb_id).toBe(412);
  expect([ue.config.tx_port, ue.config.rx_port]).toEqual([
    enb.config.rx_port,
    enb.config.tx_port,
  ]);
  // Reproduce the screenshot's manual duplicate, then apply the offered design repair.
  await page.getByLabel("Physical cell ID", { exact: true }).fill("6");
  await page.getByRole("button", { name: "Validate", exact: true }).click();
  await expect(page.locator(".validation-error")).toContainText(
    "duplicate PCI",
  );
  await expect(
    page.locator(".check-row").filter({ hasText: "Native execution adapter" }),
  ).toContainText("Supported");
  await expect(
    page.getByText("Cell 02: PCI 6 → 1", { exact: true }),
  ).toBeVisible();
  await page
    .getByRole("button", {
      name: "Apply suggested PCIs to design",
      exact: true,
    })
    .click();
  await expect(
    page.getByLabel("Physical cell ID", { exact: true }),
  ).toHaveValue("1");
  await page.getByRole("button", { name: "Validate", exact: true }).click();
  await expect(page.locator(".validation-error")).toHaveCount(0);
  const after = await (
    await page.request.get(`/api/topologies/${before.id}`)
  ).json();
  expect(
    after.nodes.find((n: { id: string }) => n.id === enb.id).cells,
  ).toEqual(enb.cells);
  await page
    .getByRole("button", { name: "Start LTE network", exact: true })
    .click();
  try {
    await expect(
      page.getByText("NAS evidence verified", { exact: true }),
    ).toBeVisible({ timeout: 110000 });
    let native: any;
    await expect
      .poll(
        async () => {
          native = await (await page.request.get("/api/simulations")).json();
          return (
            native.serving_cell?.pci === 6 &&
            native.run.broker?.data?.paths?.length === 2 &&
            native.run.broker.data.paths.every(
              (p: {
                input_samples: number;
                output_samples: number;
                input_energy: number;
              }) =>
                p.input_samples > 0 &&
                p.output_samples > 0 &&
                p.input_energy > 0,
            )
          );
        },
        { timeout: 15000 },
      )
      .toBe(true);
    expect(native.serving_cell.cell_id).toBe(enb.cells[0].id);
    expect(native.node_components[enb.id].pid).toBeGreaterThan(0);
    expect(native.components.broker.pid).toBeGreaterThan(0);
    await page.screenshot({
      path: "../docs/pci-repair-native-runtime.png",
      fullPage: true,
    });
    const fs = await import("node:fs/promises");
    await fs.writeFile(
      "../docs/pci-repair-acceptance.json",
      JSON.stringify(
        {
          topology_id: before.id,
          run_id: native.run.id,
          configured_pcis: [6, 1],
          enb_id: 412,
          serving_cell: native.serving_cell,
          broker: native.run.broker.data,
          actual_native_attachment: true,
          cell_identity_preserved: true,
          owned_process_pid: native.node_components[enb.id].pid,
        },
        null,
        2,
      ) + "\n",
    );
    expect(errors).toEqual([]);
  } finally {
    await page.request.post("/api/simulations/stop", {
      headers: { Origin: "http://127.0.0.1:3000" },
    });
  }
});
