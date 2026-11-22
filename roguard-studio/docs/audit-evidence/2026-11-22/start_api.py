import os,sys,secrets,socket,json
from pathlib import Path
root=Path('/tmp/studio-audit-20261122');studio=root/'work/srsRAN_4G/roguard-studio'
os.umask(0o077)
for port in (18180,13180):
 with socket.socket() as s:s.bind(('127.0.0.1',port))
credential=root/'private-integration-token'
credential.write_text(secrets.token_urlsafe(40));credential.chmod(0o600)
os.environ.update(STUDIO_DATA_DIR=str(studio/'var/api-audit'),STUDIO_INTEGRATION_TOKEN=credential.read_text(),ROGUARD_BACKEND_PORT='18180',ROGUARD_FRONTEND_PORT='13180')
sys.path.insert(0,str(studio/'backend'))
import uvicorn
from app.main import app
uvicorn.run(app,host='127.0.0.1',port=18180,log_level='warning',access_log=False)
