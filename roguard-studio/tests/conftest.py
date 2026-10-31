import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'backend'))
import pytest
from app.models import Component, Config, Link, Topology
from app.main import create_app
from fastapi.testclient import TestClient

@pytest.fixture
def baseline():
    return Topology(nodes=[Component(id='epc',kind='epc',name='Core'),Component(id='enb',kind='enb',name='Cell'),Component(id='ue',kind='ue',name='UE',config=Config(tx_port=2001,rx_port=2000,device_id='ue'))],links=[Link(id='s1',source='epc',target='enb',kind='s1'),Link(id='rf',source='enb',target='ue',kind='zmq')])

@pytest.fixture
def client(tmp_path):
    with TestClient(create_app(tmp_path/'state'),headers={'Origin':'http://127.0.0.1:3000'}) as c:
        assert c.post('/api/session').status_code==200
        yield c
