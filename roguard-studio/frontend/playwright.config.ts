import { defineConfig } from "@playwright/test";
export default defineConfig({
  testDir: "./tests",
  timeout: 45000,
  expect: { timeout: 10000 },
  workers: 1,
  use: {
    baseURL: process.env.STUDIO_TEST_URL || "http://127.0.0.1:3000",
    viewport: { width: 1600, height: 1000 },
    launchOptions: {
      executablePath: process.env.ROGUARD_CHROMIUM || "/usr/bin/google-chrome",
      args: ["--no-sandbox"],
    },
    screenshot: "only-on-failure",
  },
  reporter: "list",
});
