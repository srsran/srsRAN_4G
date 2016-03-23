function [ fs, cfo, p_m, w2] = find_pss( x, N_id_2)
    
    c=lte_pss_zc(N_id_2);
    cc=[zeros(33,1); c; zeros(33,1)];
    ccf=[0; cc(65:128); cc(2:64)];
    ccf=sqrt(128)*conj(ifft(ccf));
    w2=conv(x,ccf);
    [m, fs]=max(abs(w2));
 
    if (fs > 128 && fs < length(x))
        y=ccf.*x(fs-128:fs-1);
        y0=y(1:64);
        y1=y(65:length(y));    
    
        cfo=angle(conj(sum(y0))*sum(y1))/pi;
        p_m = m/mean(abs(w2));
    end
end

