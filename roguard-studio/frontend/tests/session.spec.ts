import { test, expect } from "@playwright/test";
import { api } from "../src/lib/studio";

const originalFetch = globalThis.fetch;
test.afterEach(() => {
  globalThis.fetch = originalFetch;
});

test("concurrent requests renew an expired session once and preserve save data", async () => {
  let authenticated = false;
  let sessions = 0;
  const saves: RequestInit[] = [];
  globalThis.fetch = async (url, options) => {
    if (url === "/api/session") {
      sessions++;
      await new Promise((resolve) => setTimeout(resolve, 20));
      authenticated = true;
      return Response.json({ authenticated: true });
    }
    if (!authenticated)
      return Response.json({ detail: "Local session required" }, { status: 401 });
    if (options?.method === "PUT") saves.push(options);
    return Response.json({ ok: true });
  };
  await expect(Promise.all([
    api("/system"),
    api("/simulations"),
    api("/topologies/default", "PUT", { name: "Unsaved design" }),
  ])).resolves.toEqual([{ ok: true }, { ok: true }, { ok: true }]);
  expect(sessions).toBe(1);
  expect(saves).toHaveLength(1);
  expect(saves[0].body).toBe(JSON.stringify({ name: "Unsaved design" }));
  expect(saves[0].credentials).toBe("same-origin");
});

test("persistent unauthorized responses retry only once", async () => {
  const calls: unknown[] = [];
  globalThis.fetch = async (url) => {
    calls.push(url);
    return url === "/api/session"
      ? Response.json({ authenticated: true })
      : Response.json({ detail: "Local session required" }, { status: 401 });
  };
  await expect(api("/system")).rejects.toThrow("Local session required");
  expect(calls).toEqual(["/api/system", "/api/session", "/api/system"]);
});

test("failed renewal surfaces the error and permits a later attempt", async () => {
  let sessions = 0;
  globalThis.fetch = async (url) => {
    if (url === "/api/session") sessions++;
    return Response.json({ detail: "Local session required" }, { status: 401 });
  };
  await expect(api("/system")).rejects.toThrow("Local session required");
  await expect(api("/system")).rejects.toThrow("Local session required");
  expect(sessions).toBe(2);
});

test("forbidden requests do not trigger session renewal", async () => {
  let requests = 0;
  globalThis.fetch = async () => {
    requests++;
    return Response.json({ detail: "Untrusted origin" }, { status: 403 });
  };
  await expect(api("/system")).rejects.toThrow("Untrusted origin");
  expect(requests).toBe(1);
});
