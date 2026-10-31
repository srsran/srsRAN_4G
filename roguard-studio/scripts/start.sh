#!/usr/bin/env bash
set -euo pipefail
studio_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
exec "$studio_dir/.venv/bin/python" "$studio_dir/scripts/start.py"
