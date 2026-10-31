import type { NextConfig } from "next";
const config: NextConfig = {
  distDir: process.env.ROGUARD_FRONTEND_PORT ? `.next-studio-${process.env.ROGUARD_FRONTEND_PORT}` : ".next",
  async rewrites() {
    return [
      {
        source: "/api/:path*",
        destination: `http://127.0.0.1:${process.env.ROGUARD_BACKEND_PORT || "8000"}/api/:path*`,
      },
    ];
  },
  poweredByHeader: false,
};
export default config;
