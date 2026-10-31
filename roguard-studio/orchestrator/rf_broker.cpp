// Real FC32 ZeroMQ I/Q combiner/fan-out for the installed srsRAN REQ/REP RF driver.
#include <zmq.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <complex>
#include <condition_variable>
#include <csignal>
#include <cstring>
#include <deque>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
using IQ = std::complex<float>;
constexpr size_t BLOCK = 23040, CAPACITY = BLOCK * 128;
std::atomic<bool> stopping{false}, failed{false};
std::atomic<int> bound{0};
std::mutex output_mutex;
void stop_signal(int) { stopping = true; }
struct Buffer {
  std::mutex mutex; std::condition_variable changed;
  std::deque<std::vector<IQ>> queue; size_t count=0, offset=0;
  void push(const std::vector<IQ>& data) {
    if (data.size()>CAPACITY) throw std::runtime_error("I/Q frame exceeds bounded buffer");
    std::unique_lock<std::mutex> lock(mutex);
    while (!stopping && count+data.size()>CAPACITY) changed.wait_for(lock,std::chrono::milliseconds(100));
    if (stopping) return;
    queue.push_back(data); count+=data.size(); changed.notify_all();
  }
  std::vector<IQ> take(size_t n) {
    std::unique_lock<std::mutex> lock(mutex);
    while (!stopping && count<n) changed.wait_for(lock,std::chrono::milliseconds(100));
    if (stopping) return {};
    std::vector<IQ> result(n); size_t copied=0;
    while (copied<n) {
      auto& front=queue.front(); size_t part=std::min(n-copied,front.size()-offset);
      std::copy_n(front.data()+offset,part,result.data()+copied);
      copied+=part; offset+=part; count-=part;
      if (offset==front.size()) { queue.pop_front(); offset=0; }
    }
    changed.notify_all(); return result;
  }
};
struct Socket {
  void* value;
  Socket(void* context,int type,int port,bool bind) {
    value=zmq_socket(context,type);
    if (!value) throw std::runtime_error("zmq_socket failed");
    int zero=0, timeout=200;
    zmq_setsockopt(value,ZMQ_LINGER,&zero,sizeof(zero));
    zmq_setsockopt(value,ZMQ_RCVTIMEO,&timeout,sizeof(timeout));
    zmq_setsockopt(value,ZMQ_SNDTIMEO,&timeout,sizeof(timeout));
    std::string address="tcp://127.0.0.1:"+std::to_string(port);
    int result=bind ? zmq_bind(value,address.c_str()):zmq_connect(value,address.c_str());
    if (result) { zmq_close(value); throw std::runtime_error("I/Q endpoint unavailable: "+address); }
    if (bind) ++bound;
  }
  ~Socket() { zmq_close(value); }
  bool send(const void* data,size_t size) {
    auto start=std::chrono::steady_clock::now();
    while (!stopping) {
      if (zmq_send(value,data,size,0)==static_cast<int>(size)) return true;
      if (zmq_errno()!=EAGAIN && zmq_errno()!=EINTR) throw std::runtime_error("I/Q send failed");
      if (std::chrono::steady_clock::now()-start>std::chrono::seconds(40)) throw std::runtime_error("I/Q send timed out");
    }
    return false;
  }
  std::vector<IQ> receive() {
    auto start=std::chrono::steady_clock::now();
    while (!stopping) {
      zmq_msg_t message; zmq_msg_init(&message);
      int n=zmq_msg_recv(&message,value,0);
      if (n>=0) {
        if (!n || n%sizeof(IQ) || static_cast<size_t>(n)>CAPACITY*sizeof(IQ)) { zmq_msg_close(&message); throw std::runtime_error("Invalid FC32 I/Q frame"); }
        std::vector<IQ> data(n/sizeof(IQ)); std::memcpy(data.data(),zmq_msg_data(&message),n); zmq_msg_close(&message); return data;
      }
      zmq_msg_close(&message);
      if (zmq_errno()!=EAGAIN && zmq_errno()!=EINTR) throw std::runtime_error("I/Q receive failed");
      if (std::chrono::steady_clock::now()-start>std::chrono::seconds(40)) throw std::runtime_error("I/Q source timed out");
    }
    return {};
  }
  bool request() {
    unsigned char dummy; auto start=std::chrono::steady_clock::now();
    while (!stopping) {
      if (zmq_recv(value,&dummy,1,0)>=0) return true;
      if (zmq_errno()!=EAGAIN && zmq_errno()!=EINTR) throw std::runtime_error("I/Q request failed");
      if (std::chrono::steady_clock::now()-start>std::chrono::seconds(40)) throw std::runtime_error("I/Q consumer timed out");
    }
    return false;
  }
};
int port(const char* s) { int p=std::stoi(s); if(p<1024 || p>65535) throw std::runtime_error("Invalid port"); return p; }
int main(int argc,char** argv) {
  try {
    if (argc<6 || (argc-3)%3) throw std::runtime_error("Usage: rf_broker UE_TX UE_RX [ENB_TX BROKER_UL ATTENUATION_DB]...");
    std::signal(SIGINT,stop_signal); std::signal(SIGTERM,stop_signal);
    int ue_tx=port(argv[1]), ue_rx=port(argv[2]), paths=(argc-3)/3;
    if(paths<2 || paths>3) throw std::runtime_error("Only two or three co-channel paths are supported");
    void* context=zmq_ctx_new(); if(!context) throw std::runtime_error("Cannot create ZeroMQ context");
    std::vector<Buffer> dl(paths),ul(paths);
    std::vector<float> gains; std::vector<std::atomic<uint64_t>> source_count(paths),sink_count(paths);
    std::vector<std::atomic<double>> energy(paths);
    for(int i=0;i<paths;i++) {source_count[i]=0;sink_count[i]=0;energy[i]=0;double db=std::stod(argv[5+i*3]); if(!std::isfinite(db)||db<0||db>80)throw std::runtime_error("Invalid attenuation"); gains.push_back(std::pow(10.,-db/20.));}
    std::atomic<uint64_t> dl_count{0},ul_count{0};
    std::vector<std::thread> workers;
    auto launch=[&](auto fn) { workers.emplace_back([&,fn]{try {fn();}catch(const std::exception& e){std::lock_guard<std::mutex> lock(output_mutex);std::cerr<<e.what()<<std::endl;failed=true;stopping=true;}}); };
    for(int i=0;i<paths;i++) {
      int tx=port(argv[3+i*3]),rx=port(argv[4+i*3]);
      launch([&,i,tx] {Socket s(context,ZMQ_REQ,tx,false);unsigned char request=0xff;while(!stopping){if(!s.send(&request,1))break;auto data=s.receive();source_count[i]+=data.size();double sum=0;for(auto v:data)sum+=std::norm(v);energy[i]=energy[i].load()+sum;dl[i].push(data);}});
      launch([&,i,rx] {Socket s(context,ZMQ_REP,rx,true);while(s.request()){auto data=ul[i].take(BLOCK);if(data.empty())break;for(auto& v:data)v*=gains[i];if(!s.send(data.data(),data.size()*sizeof(IQ)))break;sink_count[i]+=data.size();}});
    }
    launch([&]{Socket s(context,ZMQ_REP,ue_rx,true);while(s.request()){std::vector<IQ> mixed(BLOCK,IQ{});for(int i=0;i<paths;i++){auto data=dl[i].take(BLOCK);if(data.empty())return;for(size_t k=0;k<BLOCK;k++)mixed[k]+=data[k]*gains[i];}if(!s.send(mixed.data(),mixed.size()*sizeof(IQ)))break;dl_count+=BLOCK;}});
    launch([&]{Socket s(context,ZMQ_REQ,ue_tx,false);unsigned char request=0xff;while(!stopping){if(!s.send(&request,1))break;auto data=s.receive();if(data.empty())break;ul_count+=data.size();for(auto& buffer:ul)buffer.push(data);}});
    bool ready=false;
    while(!stopping) {
      if(!ready && bound==paths+1){std::lock_guard<std::mutex> lock(output_mutex);std::cout<<"{\"event\":\"ready\",\"paths\":"<<paths<<",\"sample_rate\":23040000,\"format\":\"fc32\"}"<<std::endl;ready=true;}
      for(int tick=0;tick<20&&!stopping;tick++)std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::lock_guard<std::mutex> lock(output_mutex);
      std::cout<<"{\"event\":\"samples\",\"downlink_samples\":"<<dl_count<<",\"uplink_samples\":"<<ul_count<<",\"paths\":[";
      for(int i=0;i<paths;i++){if(i)std::cout<<",";std::cout<<"{\"input_samples\":"<<source_count[i]<<",\"output_samples\":"<<sink_count[i]<<",\"input_energy\":"<<energy[i]<<",\"amplitude_gain\":"<<gains[i]<<"}";}
      std::cout<<"]}"<<std::endl;
    }
    for(auto& worker:workers)worker.join();zmq_ctx_term(context);return failed?1:0;
  } catch(const std::exception& e) {std::cerr<<e.what()<<std::endl;return 1;}
}
