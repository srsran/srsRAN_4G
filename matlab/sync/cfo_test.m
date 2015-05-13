clear;
sym_len=128;
hflen = (sym_len/128)*1920*10;
N_id_2=1;
input=read_complex('../../build/lte_signal.dat', hflen*500);

cp0_len=160*sym_len/2048;
cp1_len=144*sym_len/2048;

subframes=reshape(input,hflen,[]);
[n m]=size(subframes);

cfo=zeros(m,2);
for i=1:m
    cfo(i,1)      = cfo_estimate_cp(subframes(1:960,i),1,sym_len,cp0_len,cp1_len);
    [~, cfo(i,2)] = find_pss(subframes(:,i),N_id_2);

end

plot(cfo)
legend('CP-based','PSS-based')