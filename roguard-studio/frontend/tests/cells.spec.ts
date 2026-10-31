import { test, expect } from "@playwright/test";

test("owned cells persist, preview broker files and reject unsupported four-cell execution", async ({
  page,
}) => {
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
    .fill("Owned cell browser audit");
  await page
    .getByRole("button", { name: "Create project", exact: true })
    .click();
  await expect(page.locator(".library-items button")).toHaveCount(3);
  for (const name of ["Evolved Packet Core", "LTE eNodeB", "User Equipment"]) {
    await page
      .locator(".library-items button")
      .filter({ hasText: name })
      .click();
  }
  await expect(page.locator(".react-flow__edge")).toHaveCount(2);
  await page.getByTitle("Automatic layout", { exact: true }).click();
  await page.locator(".kind-enb .owned-cell").click();
  await expect(
    page.locator(".panel-heading").filter({ hasText: "CELL CONFIGURATION" }),
  ).toBeVisible();
  await page.getByLabel("Physical cell ID", { exact: true }).fill("6");
  await page.getByRole("button", { name: "Native files", exact: true }).click();
  await page
    .getByRole("button", { name: "Generate native preview", exact: true })
    .click();
  const rr = page
    .locator(".inspector details")
    .filter({ has: page.locator("summary").filter({ hasText: /^rr.conf$/ }) });
  await rr.locator("summary").click();
  await expect(rr.locator("pre")).toContainText("pci = 6;");
  await expect(rr.locator("pre")).toContainText("rf_port = 0;");
  await page.getByRole("button", { name: "Save project", exact: true }).click();
  await expect(page.getByRole("status")).toContainText("Project saved");
  await expect(page.locator(".unsaved-dot")).toHaveCount(0);
  await page.reload();
  await expect(page.locator(".kind-enb .owned-cell")).toContainText("PCI 6");
  await page.getByTitle("Automatic layout", { exact: true }).click();
  // Hold a real save response while editing; the later edit must survive.
  await page.locator(".kind-enb .owned-cell").click();
  let releaseSave: () => void = () => {};
  const heldSave = new Promise<void>((resolve) => {
    releaseSave = resolve;
  });
  let savedUrl = "";
  await page.route("**/api/topologies/*", async (route) => {
    if (route.request().method() !== "PUT") return route.continue();
    const response = await route.fetch();
    savedUrl = route.request().url();
    await heldSave;
    await route.fulfill({ response });
  });
  await page.getByRole("button", { name: "Save project", exact: true }).click();
  await expect.poll(() => savedUrl).not.toBe("");
  await page.getByLabel("Physical cell ID", { exact: true }).fill("11");
  releaseSave();
  await expect(
    page.getByRole("button", { name: "Save project", exact: true }),
  ).toBeEnabled();
  await expect(
    page.getByLabel("Physical cell ID", { exact: true }),
  ).toHaveValue("11");
  await expect(page.locator(".unsaved-dot")).toHaveCount(1);
  await page.unroute("**/api/topologies/*");
  await page.getByRole("button", { name: "Save project", exact: true }).click();
  await expect(page.locator(".unsaved-dot")).toHaveCount(0);
  const stored = await (await page.request.get(savedUrl)).json();
  expect(
    stored.nodes.find((n: { kind: string }) => n.kind === "enb").cells[0].config
      .pci,
  ).toBe(11);
  await page.locator(".kind-enb .add-cell").click();
  await expect(page.locator(".kind-enb .owned-cell")).toHaveCount(2);
  await page.getByRole("button", { name: "Validate", exact: true }).click();
  await expect(page.locator(".validation-error")).toHaveCount(0);
  await expect(
    page.locator(".check-row").filter({ hasText: "Topology configuration" }),
  ).toContainText("Passed");
  await expect(
    page.locator(".check-row").filter({ hasText: "Runtime prerequisites" }),
  ).toContainText("Passed");
  await expect(
    page.locator(".check-row").filter({ hasText: "Native execution adapter" }),
  ).toContainText("Supported");
  await expect(page.locator(".adapter-blockers")).toHaveCount(0);
  await page
    .getByRole("button", { name: "Network Builder", exact: true })
    .click();
  await expect(page.locator(".kind-enb .owned-cell")).toHaveCount(2);
  await page.getByRole("button", { name: "Native files", exact: true }).click();
  await page
    .getByRole("button", { name: "Generate native preview", exact: true })
    .click();
  await expect(
    page.getByText("Native co-channel broker generation", {
      exact: false,
    }),
  ).toBeVisible();
  for (let extra = 0; extra < 2; extra++) {
    await page.locator(".kind-enb .add-cell").click();
    await page.getByTitle("Fit topology", { exact: true }).click();
  }
  await page.getByRole("button", { name: "Validate", exact: true }).click();
  await expect(page.locator(".adapter-blockers")).toContainText(
    "two or three total cells",
  );
  await expect(page.locator(".runtime-indicator")).not.toContainText(
    "Simulation running",
  );
  await page
    .getByRole("button", { name: "Mobility Experiments", exact: true })
    .click();
  await expect(page.getByText(/PLANNED ONLY/)).toBeVisible();
  await page.getByLabel("Planned speed · design units/s").fill("4");
  await page.getByRole("button", { name: "Save project", exact: true }).click();
  await expect(page.getByRole("status")).toContainText("Project saved");
  await page
    .getByRole("button", { name: "Radio Environment", exact: true })
    .click();
  await expect(page.getByLabel("Designed radio environment")).toBeVisible();
  await expect(
    page.getByText("No current resolved serving cell.", { exact: false }),
  ).toBeVisible();
  expect(errors).toEqual([]);
});
