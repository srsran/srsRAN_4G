function [ corr ] = ifo_pss_corr( n, r_pss, x_pss)

    x=0;
    for i=1:length(x_pss)
        x=x+r_pss(1+mod(i+n-1,length(r_pss)))*conj(x_pss(i));
    end
    corr=real(exp(1i*2*pi*9*n/128)*x);
%    corr=abs(x);
end

