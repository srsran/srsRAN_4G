"""Real compiled broker with synthetic FC32 fixtures, not native LTE evidence."""
from array import array
import subprocess
import threading
import time
import pytest
from test_rf_broker import ZMQ,broker_binary,free_endpoint


def test_independent_ue_uplinks_and_receive_matrices(broker_binary,tmp_path):
    z=ZMQ();stop=threading.Event();servers=[z.socket(4) for _ in range(4)];outputs=[free_endpoint(z) for _ in range(4)]
    manifest=tmp_path/'matrix.txt'
    manifest.write_text(f'1920000 2 2\n{servers[0][1]} {outputs[0]}\n{servers[1][1]} {outputs[1]}\n{servers[2][1]} {outputs[2]} 0 20\n{servers[3][1]} {outputs[3]} 40 0\n')
    errors=[];results={}
    def serve(which):
        s=servers[which][0];counter=0
        try:
            while not stop.is_set():
                if z.recv(s) is None:continue
                n=[4095,12345,9001,3071][which];data=array('f')
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
    workers=[threading.Thread(target=serve,args=(i,)) for i in range(4)]
    consumers=[threading.Thread(target=consume,args=(i,)) for i in range(4)]
    process=subprocess.Popen([str(broker_binary),'--matrix',str(manifest)],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    try:
        for task in workers+consumers:task.start()
        for task in consumers:task.join(12)
        assert not errors and len(results)==4,errors
        # Cell UL outputs are independent UE sums, not duplicated UE1 data.
        gains=[(3.04,-4.05),(4.3,-5.4),(1.2,-2.3),(2.01,-3.02)]
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


@pytest.mark.parametrize('manifest',['12345 1 1\n2000 2001\n2002 2003 0\n','1920000 1 1\n2000 2000\n2002 2003 0\n','1920000 1 1\n2000 2001\n2002 2003 nan\n'])
def test_invalid_matrix_fails_before_opening_transports(broker_binary,tmp_path,manifest):
    path=tmp_path/'bad';path.write_text(manifest)
    result=subprocess.run([str(broker_binary),'--matrix',str(path)],capture_output=True,timeout=5)
    assert result.returncode==1 and b'Invalid' in result.stderr
