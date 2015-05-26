clear;
sym_len=128;
hflen = (sym_len/128)*1920*5;
samp_rate = (sym_len/128)*1920000;
N_id_2=1;
input=read_complex('../../build/lte_signal.dat', hflen*200);

cp0_len=160*sym_len/2048;
cp1_len=144*sym_len/2048;

%t = (0:length(input)-1).'/samp_rate;
%input = input .* exp(-1i*2*pi*2000.0*t);

subframes=reshape(input,hflen,[]);
[n m]=size(subframes);

cfdl=struct('NDLRB',6,'CyclicPrefix','Normal','DuplexMode','FDD');
cfo=zeros(m,2);
for i=1:m
    [toffset, cfo(i,2)] = find_pss(subframes(:,i),N_id_2);
    cfo(i,1)            = lteFrequencyOffset(cfdl,subframes(:,i),toffset)/15000;
end

plot(cfo*15000)
legend('Matlab','PSS-based')
disp(mean(cfo)*15)
