function [ y ] = addnoise( x, snr_db )
v = 10^(-snr_db/10);
y=x+sqrt(v)*(randn(size(x))+1i*randn(size(x)))/sqrt(2);
y=y/sqrt(mean(y.*conj(y)));
end

