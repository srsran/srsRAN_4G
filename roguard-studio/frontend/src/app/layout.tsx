import type { Metadata } from "next";
import "@fontsource/inter/400.css";
import "@fontsource/inter/500.css";
import "@fontsource/inter/600.css";
import "@fontsource/jetbrains-mono/400.css";
import "@xyflow/react/dist/style.css";
import "./globals.css";
export const metadata: Metadata = {
  title: "srsRAN Studio · LTE Workbench",
  description:
    "Local LTE topology design, isolated srsRAN orchestration and research evidence.",
};
export default function RootLayout({
  children,
}: Readonly<{ children: React.ReactNode }>) {
  return (
    <html lang="en">
      <body>{children}</body>
    </html>
  );
}
