"""Real compiled broker with synthetic FC32 fixtures, not native LTE evidence."""
from array import array
import json
import subprocess
import threading
import time
import pytest
from test_rf_broker import ZMQ,broker_binary,free_endpoint


@pytest.mark.parametrize('ue_count',[2,3,5])
def test_independent_ue_uplinks_and_receive_matrices(broker_binary,tmp_path,ue_count):
    count=2+ue_count
    z=ZMQ();stop=threading.Event();servers=[z.socket(4) for _ in range(count)];outputs=[free_endpoint(z) for _ in range(count)]
    attenuation=[[0,20] if i%2==0 else [40,0] for i in range(ue_count)]
    manifest=tmp_path/'matrix.txt'
    lines=[f'1920000 2 {ue_count}',*[f'{servers[i][1]} {outputs[i]}' for i in range(2)]]
    lines += [f'{servers[i+2][1]} {outputs[i+2]} '+ ' '.join(map(str,db)) for i,db in enumerate(attenuation)]
    manifest.write_text('\n'.join(lines)+'\n')
    errors=[];results={}
    def serve(which):
        s=servers[which][0];counter=0
        try:
            while not stop.is_set():
                if z.recv(s) is None:continue
                n=[4095,12345,9001,3071][which%4];data=array('f')
                for k in range(counter,counter+n):
                    v=k%17+1;data.extend(((which+1)*v,-(which+2)*v))
                if not z.send(s,data.tobytes()):break
                counter+=n
        finally:z.lib.zmq_close(s)
    def consume(index):
        s=z.socket(3,outputs[index]);data=array('f')
        try:
            for _ in range(3):
                assert z.send(s,b'\xff');deadline=time.monotonic()+10
                while time.monotonic()<deadline:
                    frame=z.recv(s)
                    if frame is not None:break
                else:raise AssertionError('No independent matrix output')
                data.frombytes(frame)
            results[index]=data
        except BaseException as exc:errors.append(exc)
        finally:z.lib.zmq_close(s)
    workers=[threading.Thread(target=serve,args=(i,)) for i in range(count)]
    consumers=[threading.Thread(target=consume,args=(i,)) for i in range(count)]
    process=subprocess.Popen([str(broker_binary),'--matrix',str(manifest)],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    try:
        for task in workers+consumers:task.start()
        for task in consumers:task.join(12)
        assert not errors and len(results)==count,errors
        # Cell UL outputs are independent UE sums, not duplicated UE1 data.
        weights=[[10**(-db/20) for db in row] for row in attenuation]
        gains=[(sum((u+3)*weights[u][c] for u in range(ue_count)),-sum((u+4)*weights[u][c] for u in range(ue_count))) for c in range(2)]
        gains += [(w[0]+2*w[1],-2*w[0]-3*w[1]) for w in weights]
        for output,(real,imag) in enumerate(gains):
            assert len(results[output])==1920*3*2
            for k in (0,16,17,1919,1920,4094,4095,5759):
                v=k%17+1;assert results[output][2*k]==pytest.approx(real*v,abs=.0001)
                assert results[output][2*k+1]==pytest.approx(imag*v,abs=.0001)
    finally:
        process.terminate();process.wait(10);stop.set()
        for task in consumers+workers:task.join(3)
        z.lib.zmq_ctx_term(z.ctx)
    assert process.returncode==0,process.stderr.read().decode()
    events=[json.loads(line) for line in process.stdout.read().decode().splitlines()]
    diagnostics=[event for event in events if event['event']=='samples'][-1]
    assert len(diagnostics['buffers'])==2*count
    assert all(0<=b['depth_samples']<=b['high_water_samples']<=b['capacity_samples'] for b in diagnostics['buffers'])
    assert diagnostics['sequence_gaps'] is None
    assert len(diagnostics['path_gains'])==2*ue_count
    for path in diagnostics['path_gains']:
        assert path['amplitude_gain']==pytest.approx(weights[path['ue_index']][path['cell_index']])



@pytest.mark.parametrize('manifest',['1920000 1 999999999\n','12345 1 1\n2000 2001\n2002 2003 0\n','1920000 1 1\n2000 2000\n2002 2003 0\n','1920000 1 1\n2000 2001\n2002 2003 nan\n'])
def test_invalid_matrix_fails_before_opening_transports(broker_binary,tmp_path,manifest):
    path=tmp_path/'bad';path.write_text(manifest)
    result=subprocess.run([str(broker_binary),'--matrix',str(path)],capture_output=True,timeout=5)
    assert result.returncode==1 and b'Invalid' in result.stderr
