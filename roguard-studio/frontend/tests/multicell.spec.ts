import { test, expect } from "@playwright/test";

test("two native eNodeBs attach through the real I/Q broker", async ({
  page,
}) => {
  test.setTimeout(150000);
  const errors: string[] = [];
  page.on("pageerror", (e) => errors.push(e.message));
  await page.goto("/");
  await expect(
    page.getByText("Engine connected", { exact: true }),
  ).toBeVisible();
  await page.locator(".project-switcher>button").click();
  await page.getByRole("button", { name: "New project", exact: true }).click();
  await page
    .getByLabel("Project name", { exact: true })
    .fill("Actual two-eNodeB broker verification");
  await page
    .getByRole("button", { name: "Create project", exact: true })
    .click();
  for (const label of [
    "Evolved Packet Core",
    "LTE eNodeB",
    "LTE eNodeB",
    "User Equipment",
  ])
    await page
      .locator(".library-items button")
      .filter({ hasText: label })
      .click();
  await expect(page.locator(".network-node")).toHaveCount(4);
  await expect(page.locator(".react-flow__edge")).toHaveCount(4);
  await page.getByRole("button", { name: "Validate", exact: true }).click();
  await expect(page.locator(".adapter-blockers")).toHaveCount(0);
  await expect(
    page.locator(".check-row").filter({ hasText: "Native execution adapter" }),
  ).toContainText("Supported");
  await page
    .getByRole("button", { name: "Start LTE network", exact: true })
    .click();
  try {
    await expect(
      page.getByText("NAS evidence verified", { exact: true }),
    ).toBeVisible({ timeout: 110000 });
    await expect(page.locator(".process-row")).toHaveCount(5);
    await expect(page.locator('[data-component-id="broker"]')).toContainText(
      /PID \d+/,
    );
    await expect(page.locator(".runtime-indicator")).toContainText(
      "Simulation running",
    );
    let native: any;
    await expect
      .poll(
        async () => {
          native = await (await page.request.get("/api/simulations")).json();
          return native.serving_cell?.pci;
        },
        { timeout: 15000 },
      )
      .toBe(1);
    const enbs = native.run.topology.nodes.filter(
      (n: { kind: string }) => n.kind === "enb",
    );
    expect(
      new Set(enbs.map((n: { id: string }) => native.node_components[n.id].pid))
        .size,
    ).toBe(2);
    await expect.poll(async () => {
      native = await (await page.request.get("/api/simulations")).json();
      return native.run.broker?.data.paths?.length === 2 && native.run.broker.data.paths.every((p: {input_samples:number;input_energy:number})=>p.input_samples>0 && p.input_energy>0);
    },{timeout:15000}).toBe(true);
    await page.screenshot({
      path: "../docs/multicell-native-runtime.png",
      fullPage: true,
    });
    await page
      .getByRole("button", { name: "ZeroMQ / RF Transport", exact: true })
      .click();
    await expect(
      page.getByRole("heading", {
        name: "Applied I/Q broker routing",
        exact: true,
      }),
    ).toBeVisible();
    await expect(
      page
        .locator(".surface")
        .filter({
          has: page.getByRole("heading", {
            name: "Applied I/Q broker routing",
          }),
        })
        .getByRole("row"),
    ).toHaveCount(3);
    expect(errors).toEqual([]);
  } finally {
    await page.request.post("/api/simulations/stop", {
      headers: { Origin: "http://127.0.0.1:3000" },
    });
  }
});
