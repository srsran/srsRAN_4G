"""Diagnostics are actual queue state, not inferred RF health."""
import json
import subprocess
from pathlib import Path


def test_bounded_buffer_alignment_and_diagnostics(tmp_path):
    source=Path(__file__).resolve().parents[1]/'orchestrator/rf_broker.cpp'
    unit=tmp_path/'buffer.cpp'
    unit.write_text('''#define main broker_entry
#include "'''+str(source)+'''"
#undef main
#include <cassert>
#include <sstream>
int main() {
  Buffer b;
  b.push({IQ(1,2),IQ(3,4)}); b.push({IQ(5,6)});
  auto first=b.take(1); auto rest=b.take(2);
  assert(first[0]==IQ(1,2)); assert(rest[0]==IQ(3,4)); assert(rest[1]==IQ(5,6));
  b.diagnostics(std::cout);
  bool rejected=false;
  try {b.push(std::vector<IQ>(CAPACITY+1));} catch(const std::runtime_error&) {rejected=true;}
  assert(rejected);
}
''')
    binary=tmp_path/'buffer'
    subprocess.run(['g++','-std=c++17','-O2','-pthread',str(unit),'-lzmq','-o',str(binary)],check=True)
    result=subprocess.run([str(binary)],check=True,capture_output=True,text=True)
    data=json.loads(result.stdout)
    assert data['depth_samples']==0
    assert data['high_water_samples']==3
    assert data['capacity_samples']==23040*128
    assert data['producer_wait_intervals']==data['consumer_wait_intervals']==0


def test_diagnostic_history_is_run_scoped_and_authenticated(client):
    from app.storage import now
    assert client.get('/api/zeromq/diagnostics',cookies={'rg_session':'invalid'}).status_code==401
    store=client.app.state.store
    for run in ('a','b'):
        store.put('broker-diagnostic',run,{'run_id':run,'timestamp':now(),'component':'broker-2','data':{'event':'samples'}})
    response=client.get('/api/zeromq/diagnostics?run_id=a').json()
    assert response['current'] is False
    assert [e['run_id'] for e in response['history']]==['a']
    assert client.get('/api/zeromq/diagnostics?run_id=unknown').json()['history']==[]
