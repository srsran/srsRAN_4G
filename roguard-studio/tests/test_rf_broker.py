"""Binary protocol tests using explicit FC32 fixtures, not native LTE evidence."""
from array import array
import ctypes as C
from ctypes.util import find_library
from pathlib import Path
import subprocess
import sys
import threading
import time
import pytest

STUDIO=Path(__file__).resolve().parents[1]
class ZMQ:
    def __init__(self):
        self.lib=C.CDLL(find_library('zmq'))
        signatures={'zmq_ctx_new':([],C.c_void_p),'zmq_ctx_term':([C.c_void_p],C.c_int),
                    'zmq_socket':([C.c_void_p,C.c_int],C.c_void_p),'zmq_close':([C.c_void_p],C.c_int),
                    'zmq_bind':([C.c_void_p,C.c_char_p],C.c_int),'zmq_connect':([C.c_void_p,C.c_char_p],C.c_int),
                    'zmq_setsockopt':([C.c_void_p,C.c_int,C.c_void_p,C.c_size_t],C.c_int),
                    'zmq_getsockopt':([C.c_void_p,C.c_int,C.c_void_p,C.POINTER(C.c_size_t)],C.c_int),
                    'zmq_send':([C.c_void_p,C.c_void_p,C.c_size_t,C.c_int],C.c_int),
                    'zmq_recv':([C.c_void_p,C.c_void_p,C.c_size_t,C.c_int],C.c_int)}
        for name,(args,result) in signatures.items():getattr(self.lib,name).argtypes=args;getattr(self.lib,name).restype=result
        self.ctx=self.lib.zmq_ctx_new()
    def socket(self,kind,port=None):
        s=self.lib.zmq_socket(self.ctx,kind)
        for option,value in ((17,0),(27,200),(28,200)):
            n=C.c_int(value);assert self.lib.zmq_setsockopt(s,option,C.byref(n),C.sizeof(n))==0
        if port is None:
            assert self.lib.zmq_bind(s,b'tcp://127.0.0.1:*')==0
            buf=C.create_string_buffer(256);length=C.c_size_t(256)
            assert self.lib.zmq_getsockopt(s,32,buf,C.byref(length))==0
            return s,int(buf.value.rsplit(b':',1)[1])
        assert self.lib.zmq_connect(s,f'tcp://127.0.0.1:{port}'.encode())==0
        return s
    def send(self,s,data):return self.lib.zmq_send(s,data,len(data),0)>=0
    def recv(self,s):
        buf=C.create_string_buffer(2*1024*1024);n=self.lib.zmq_recv(s,buf,len(buf),0)
        return None if n<0 else buf.raw[:n]

@pytest.fixture(scope='module')
def broker_binary():
    subprocess.run([sys.executable,str(STUDIO/'scripts/build-broker.py')],check=True)
    return STUDIO/'var/tools/rf_broker'

def free_endpoint(z):
    s,port=z.socket(4);z.lib.zmq_close(s);return port

def test_real_fc32_sum_variable_frames_and_attenuated_uplink_fanout(broker_binary):
    z=ZMQ();stop=threading.Event();workers=[];servers=[]
    for _ in range(3):servers.append(z.socket(4))
    dl1,dl2,ue=servers;output_ports=[free_endpoint(z) for _ in range(3)]
    def serve(sock,which):
        counter=0
        try:
            while not stop.is_set():
                if z.recv(sock) is None:continue
                n=[4095,12345,9001][which]
                values=array('f')
                for k in range(counter,counter+n):
                    v=k%17
                    values.extend((float(v),float(v+1)) if which==0 else (float(2*v),float(3*v)) if which==1 else (float(v/2),float(-v)))
                if not z.send(sock,values.tobytes()):break
                counter+=n
        finally:z.lib.zmq_close(sock)
    for i,(sock,_) in enumerate(servers):
        task=threading.Thread(target=serve,args=(sock,i));task.start();workers.append(task)
    process=subprocess.Popen([str(broker_binary),str(ue[1]),str(output_ports[0]),str(dl1[1]),str(output_ports[1]),'0',str(dl2[1]),str(output_ports[2]),'20'],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    results={};errors=[]
    def consume(port,key):
        s=z.socket(3,port)
        try:
            assert z.send(s,b'\xff')
            deadline=time.monotonic()+8
            while time.monotonic()<deadline:
                data=z.recv(s)
                if data is not None:
                    values=array('f');values.frombytes(data);results[key]=values;return
            raise AssertionError('Broker did not produce samples')
        except BaseException as e:errors.append(e)
        finally:z.lib.zmq_close(s)
    consumers=[threading.Thread(target=consume,args=(port,key)) for key,port in enumerate(output_ports)]
    try:
        for task in consumers:task.start()
        for task in consumers:task.join(10)
        assert not errors,errors
        assert len(results)==3
        for i in (0,16,17,4094,4095,12344,12345,23039):
            v=i%17
            assert results[0][2*i]==pytest.approx(1.2*v,abs=.00001)
            assert results[0][2*i+1]==pytest.approx(1.3*v+1,abs=.00001)
            assert results[1][2*i]==pytest.approx(v/2)
            assert results[2][2*i]==pytest.approx(v/20,abs=.00001)
            assert results[2][2*i+1]==pytest.approx(-v/10,abs=.00001)
    finally:
        process.terminate();process.wait(8);stop.set()
        for task in consumers+workers:task.join(3)
        z.lib.zmq_ctx_term(z.ctx)
    assert process.returncode==0,process.stderr.read().decode()

def test_broker_reports_listener_collision_without_disrupting_owner(broker_binary):
    z=ZMQ();owner,occupied=z.socket(4)
    process=subprocess.Popen([str(broker_binary),str(free_endpoint(z)),str(occupied),str(free_endpoint(z)),str(free_endpoint(z)),'0',str(free_endpoint(z)),str(free_endpoint(z)),'20'],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    try:
        _,err=process.communicate(timeout=8)
        assert process.returncode!=0
        assert b'endpoint unavailable' in err
        # The original listener is still owned by this test, not killed by name.
        assert z.lib.zmq_close(owner)==0
    finally:
        if process.poll() is None:process.kill();process.wait()
        z.lib.zmq_ctx_term(z.ctx)
