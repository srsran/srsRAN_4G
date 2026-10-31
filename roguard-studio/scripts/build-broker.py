#!/usr/bin/env python3
"""Build only Studio's I/Q broker, without rebuilding or changing srsRAN."""
import hashlib
import os
from pathlib import Path
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[1]
def main():
    source=ROOT/'orchestrator/rf_broker.cpp';directory=ROOT/'var/tools'
    binary=directory/'rf_broker';stamp=directory/'rf_broker.sha256'
    digest=hashlib.sha256(source.read_bytes()).hexdigest()
    if binary.is_file() and os.access(binary,os.X_OK) and stamp.is_file() and stamp.read_text()==digest:return
    directory.mkdir(parents=True,exist_ok=True,mode=0o700)
    with tempfile.TemporaryDirectory(prefix='broker-build-',dir=directory) as tmp:
        output=Path(tmp)/'rf_broker'
        subprocess.run(['g++','-std=c++17','-O2','-pthread',str(source),'-lzmq','-o',str(output)],check=True)
        output.chmod(0o700);os.replace(output,binary)
        stamp.write_text(digest)
    print('Studio FC32 I/Q broker built; existing srsRAN binaries preserved.')
if __name__=='__main__':main()
