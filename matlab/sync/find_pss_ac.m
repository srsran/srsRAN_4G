function fs = find_pss_ac( x)
    
    w2=xcorr(x,x);
    [m, fs]=max(abs(w2)); 
    fs=fs-1920;
end

