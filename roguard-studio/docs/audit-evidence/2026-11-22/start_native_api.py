import os,sys,socket
from pathlib import Path
root=Path('/tmp/studio-audit-20261122');studio=root/'work/srsRAN_4G/roguard-studio'
with socket.socket() as s:s.bind(('127.0.0.1',18181))
os.environ.update(STUDIO_DATA_DIR=str(studio/'var/native-api-audit'),STUDIO_INTEGRATION_TOKEN=(root/'private-integration-token').read_text(),ROGUARD_BACKEND_PORT='18181')
sys.path.insert(0,str(studio/'backend'))
import uvicorn
from app.main import app
uvicorn.run(app,host='127.0.0.1',port=18181,log_level='warning',access_log=False)
