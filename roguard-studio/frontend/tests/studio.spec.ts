import { test, expect } from "@playwright/test";

test.afterEach(async ({ page }) => {
  await page.request.post("/api/simulations/stop", {
    headers: { Origin: "http://127.0.0.1:3000" },
  });
});

test("editor controls, persistence, real runtime and evidence export", async ({
  page,
}) => {
  test.setTimeout(180000);
  const projectName = "Browser acceptance laboratory " + Date.now();
  const errors: string[] = [];
  page.on("pageerror", (e) => errors.push(e.message));
  await page.goto("/");
  await expect(
    page.getByText("Engine connected", { exact: true }),
  ).toBeVisible();
  // Create an actual saved design through the UI, with no fake runtime state.
  await page.locator(".project-switcher>button").click();
  await page.getByRole("button", { name: "New project", exact: true }).click();
  await page.getByLabel("Project name", { exact: true }).fill(projectName);
  await page
    .getByRole("button", { name: "Create project", exact: true })
    .click();
  await page
    .locator(".library-items button")
    .filter({ hasText: "Evolved Packet Core" })
    .click();
  await page
    .locator(".library-items button")
    .filter({ hasText: "LTE eNodeB" })
    .click();
  await page
    .locator(".library-items button")
    .filter({ hasText: "User Equipment" })
    .click();
  await expect(page.locator(".network-node")).toHaveCount(3);
  await page.getByTitle("Automatic layout", { exact: true }).click();
  // Core and reciprocal single-cell I/Q relationships are configured automatically.
  await expect(page.locator(".react-flow__edge")).toHaveCount(2);
  await page.getByRole("button", { name: "Save project", exact: true }).click();
  await expect(page.getByRole("status")).toContainText("Project saved");
  await page.reload();
  await expect(
    page.getByText("Engine connected", { exact: true }),
  ).toBeVisible();
  // Project load chooses the first saved project; reopen this one if necessary.
  await page
    .getByRole("button", { name: "Configuration Files", exact: true })
    .click();
  const project = page
    .locator(".saved-project")
    .filter({ hasText: projectName })
    .last();
  await expect(project).toBeVisible();
  await project.getByRole("button", { name: "Open", exact: true }).click();
  await expect(page.locator(".network-node")).toHaveCount(3);
  await page.getByRole("button", { name: "Validate", exact: true }).click();
  await expect(
    page.getByText("Runtime prerequisites", { exact: true }),
  ).toBeVisible();
  await expect(page.locator(".validation-error")).toHaveCount(0);
  await page
    .getByRole("button", { name: "Start LTE network", exact: true })
    .click();
  await expect(
    page.getByText("NAS evidence verified", { exact: true }),
  ).toBeVisible({ timeout: 120000 });
  await expect(page.locator(".runtime-indicator")).toContainText(
    "Simulation running",
  );
  await page
    .getByRole("button", { name: "Native Measurements", exact: true })
    .click();
  await expect(page.getByText("Live telemetry", { exact: true })).toBeVisible();
  await expect(
    page.locator(".stat-card").first().locator("strong"),
  ).not.toHaveText("—dBm", { timeout: 20000 });
  await page
    .getByRole("button", { name: "ZeroMQ / RF Transport", exact: true })
    .click();
  await expect(page.locator(".code-block")).toContainText("ESTAB", {
    timeout: 10000,
  });
  await page
    .getByRole("button", { name: "Capture experiment", exact: true })
    .click();
  await page
    .getByLabel("Experiment name", { exact: true })
    .fill("Verified browser LTE experiment");
  await page
    .getByLabel("Scenario ground truth · analyst supplied")
    .selectOption("Suspicious");
  await page
    .getByRole("button", { name: "Save snapshot", exact: true })
    .click();
  await expect(
    page
      .locator(".experiment-card")
      .filter({ hasText: "Verified browser LTE experiment" })
      .last(),
  ).toBeVisible();
  const [download] = await Promise.all([
    page.waitForEvent("download"),
    page
      .locator(".experiment-card")
      .filter({ hasText: "Verified browser LTE experiment" })
      .last()
      .getByRole("link", { name: "Export", exact: true })
      .click(),
  ]);
  expect(download.suggestedFilename()).toMatch(/\.zip$/);
  await page
    .getByRole("button", { name: "Simulation Runtime", exact: true })
    .click();
  const oldRun = await page
    .locator(".runtime-indicator")
    .getAttribute("data-run-id");
  await page
    .getByRole("button", { name: "Restart chain", exact: true })
    .click();
  await expect(page.locator(".runtime-indicator")).not.toHaveAttribute(
    "data-run-id",
    oldRun ?? "",
    { timeout: 60000 },
  );
  await expect(
    page.getByText("NAS evidence verified", { exact: true }),
  ).toBeVisible({ timeout: 120000 });
  await page.getByRole("button", { name: "Stop network", exact: true }).click();
  await expect(page.locator(".runtime-indicator")).toContainText(
    "Simulation stopped",
    { timeout: 50000 },
  );
  await page
    .getByRole("button", { name: "Network Builder", exact: true })
    .click();
  await page.screenshot({ path: "../docs/workbench-desktop.png", fullPage: true });
  expect(errors).toEqual([]);
});

test("empty state, history, grouping and responsive shell", async ({
  page,
}) => {
  await page.goto("/");
  await expect(
    page.getByText("Engine connected", { exact: true }),
  ).toBeVisible();
  await page.locator(".project-switcher>button").click();
  await page.getByRole("button", { name: "New project", exact: true }).click();
  await page
    .getByLabel("Project name", { exact: true })
    .fill("Editor interactions");
  await page
    .getByRole("button", { name: "Create project", exact: true })
    .click();
  await page
    .locator(".library-items button")
    .filter({ hasText: "LTE eNodeB" })
    .click();
  await expect(page.locator(".network-node")).toHaveCount(1);
  await page.keyboard.press("Control+d");
  await expect(page.locator(".network-node")).toHaveCount(2);
  await page.keyboard.press("Control+z");
  await expect(page.locator(".network-node")).toHaveCount(1);
  await page.keyboard.press("Control+Shift+z");
  await expect(page.locator(".network-node")).toHaveCount(2);
  await page.getByTitle("Group selected components", { exact: true }).click();
  await expect(page.locator(".namespace-node")).toHaveCount(1);
  await page.getByTitle("Collapse sidebar", { exact: true }).click();
  await expect(page.locator(".studio-app")).toHaveClass(/sidebar-collapsed/);
  await page.setViewportSize({ width: 800, height: 900 });
  await page.screenshot({ path: "../docs/workbench-compact.png", fullPage: true });
  const overflow = await page.evaluate(
    () => document.documentElement.scrollWidth > window.innerWidth,
  );
  expect(overflow).toBe(false);
});
