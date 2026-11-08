#!/usr/bin/env bash
set -euo pipefail
studio_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
if [[ ! -x "$studio_dir/.venv/bin/python" ]]; then
  cat >&2 <<EOF
Studio's Python environment is missing. From the repository root, run:
  python3 -m venv roguard-studio/.venv
  roguard-studio/.venv/bin/pip install -r roguard-studio/backend/requirements.lock.txt
  npm ci --prefix roguard-studio/frontend
Then rerun ./roguard-studio/scripts/start.sh
EOF
  exit 1
fi
exec "$studio_dir/.venv/bin/python" "$studio_dir/scripts/start.py"
