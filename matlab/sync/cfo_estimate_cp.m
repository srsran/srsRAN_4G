function cfo = cfo_estimate_cp(input, Nsyms, sym_len, cp0_len, cp_len)

r=zeros(Nsyms, 1);
r(1)=sum(input(1:cp0_len).*conj(input(1+sym_len:cp0_len+sym_len)));
s=cp0_len+sym_len+1;
e=cp0_len+sym_len+cp_len;
for i=2:Nsyms
    r(i)=sum(input(s:e).*conj(input(s+sym_len:e+sym_len)));
    s=s+sym_len+cp_len;
    e=e+sym_len+cp_len;
end
cfo=-angle(mean(r))/2/pi;