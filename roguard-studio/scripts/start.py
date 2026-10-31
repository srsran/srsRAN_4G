#!/usr/bin/env python3
"""Start the local backend and Next.js without modifying the native build."""
from pathlib import Path
import os
import errno
import fcntl
from urllib.request import build_opener, ProxyHandler
from urllib.error import URLError
import signal
import socket
import subprocess
import sys
import time

ROOT=Path(__file__).resolve().parents[1]
children=[]
def shutdown(exit_code=0):
    for p in children:
        if p.poll() is None:
            try:os.killpg(p.pid,signal.SIGTERM)
            except ProcessLookupError:pass
    for p in children:
        try:p.wait(timeout=50)
        except subprocess.TimeoutExpired:
            try:os.killpg(p.pid,signal.SIGKILL)
            except ProcessLookupError:pass
    sys.exit(exit_code)

def stop(*_):
    shutdown()

def check_workspace():
    directory = ROOT / "var"
    directory.mkdir(parents=True, exist_ok=True, mode=0o700)
    with (directory / "backend.lock").open("a") as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            sys.exit("Studio workspace is already in use by another backend. "
                     "Stop that Studio instance before restarting. No services were started.")

def wait_for_service(process, url, timeout=60):
    opener = build_opener(ProxyHandler({}))
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if process.poll() is not None:
            print(f"Studio service exited with code {process.returncode} before becoming ready.", file=sys.stderr)
            shutdown(1)
        try:
            with opener.open(url, timeout=1) as response:
                if response.status == 200:
                    return
        except (URLError, TimeoutError, ConnectionError):
            pass
        time.sleep(.2)
    print(f"Studio service did not become ready: {url}", file=sys.stderr)
    shutdown(1)

def free_port(preferred):
    for candidate in (preferred, 0):
        with socket.socket() as listener:
            # Match uvicorn/Node's reuse behavior: TIME_WAIT is not a live listener.
            listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            try:
                listener.bind(('127.0.0.1', candidate))
            except OSError as error:
                if candidate and error.errno == errno.EADDRINUSE:
                    continue
                raise
            return listener.getsockname()[1]

def main():
    if not (ROOT/'.venv/bin/python').exists() or not (ROOT/'frontend/node_modules/next').exists():
        sys.exit('Install dependencies first. See roguard-studio/README.md.')
    check_workspace()
    broker_build=subprocess.run([str(ROOT/'.venv/bin/python'),str(ROOT/'scripts/build-broker.py')])
    if broker_build.returncode:print('I/Q broker build unavailable. Single-cell mode remains available; multi-cell validation will report the missing broker.',file=sys.stderr)
    frontend_port = free_port(3000)
    backend_port = free_port(8000)
    service_env = {**os.environ, 'ROGUARD_FRONTEND_PORT': str(frontend_port),
                   'ROGUARD_BACKEND_PORT': str(backend_port), 'NEXT_TELEMETRY_DISABLED': '1'}
    for sig in (signal.SIGINT,signal.SIGTERM):signal.signal(sig,stop)
    children.append(subprocess.Popen([str(ROOT/'.venv/bin/python'),'-m','uvicorn','app.main:app','--app-dir',str(ROOT/'backend'),'--host','127.0.0.1','--port',str(backend_port),'--timeout-graceful-shutdown','5'],cwd=ROOT,start_new_session=True,env=service_env))
    wait_for_service(children[-1], f'http://127.0.0.1:{backend_port}/docs')
    children.append(subprocess.Popen(['node',str(ROOT/'frontend/node_modules/next/dist/bin/next'),'dev','--webpack','--hostname','127.0.0.1','--port',str(frontend_port)],cwd=ROOT/'frontend',start_new_session=True,env=service_env))
    wait_for_service(children[-1], f'http://127.0.0.1:{frontend_port}')
    print(f'\nRoguard Studio: http://127.0.0.1:{frontend_port}\nControl API: http://127.0.0.1:{backend_port}/docs\nCtrl+C stops Studio and its owned simulation processes.\n',flush=True)
    while all(p.poll() is None for p in children):time.sleep(.5)
    shutdown(1)
if __name__=='__main__':main()
