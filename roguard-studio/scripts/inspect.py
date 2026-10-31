#!/usr/bin/env python3
"""Read-only discovery report; never reads existing subscriber key values."""
from pathlib import Path
import hashlib
import json
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'backend'))
from app.discovery import REPO,installation
report=installation()
report['upstream_example_sha256']={str(p.relative_to(REPO)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [REPO/'srsenb/enb.conf.example',REPO/'srsue/ue.conf.example',REPO/'srsepc/epc.conf.example',REPO/'srsepc/user_db.csv.example',*[REPO/f'srsenb/{n}.conf.example' for n in ['sib','rr','rb']]]}
print(json.dumps(report,indent=2))
