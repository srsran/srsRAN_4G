function [cfo, r] = cfo_estimate_cp(input_slot, Nsyms, sym_len, cp0_len, cp_len)

% Start correlating from the end. Nsyms is the number of symbols to
% correlate starting from the end. 

s=length(input_slot)-sym_len-cp_len;
e=length(input_slot)-sym_len;
for i=1:Nsyms
    r(i)=sum(input_slot(s:e).*conj(input_slot(s+sym_len:e+sym_len)));
    if (i < 7)
        s=s-sym_len-cp_len;
        e=e-sym_len-cp_len;
    else
        s=s-sym_len-cp0_len;
        e=e-sym_len-cp0_len;
    end
end
cfo=-angle(mean(r))/pi;