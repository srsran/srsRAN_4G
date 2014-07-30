function [ w2] = find_pss2( x, N_id_2, fft_size)
    c=lte_pss_zc(N_id_2);
    cc=[zeros(fft_size/2-31,1); c; zeros(fft_size/2-31,1)];
    ccd=[0; cc(fft_size/2+1:fft_size); cc(2:fft_size/2)];
    ccf=sqrt(fft_size)*conj(ifft(ccd));
    
    w2=real(conv(x,ccf))/62;
    plot(w2) 
    [m i]=max(w2);
    en=var(x,1);
    p_m = m/en;
    
    fprintf('Frame starts at %d, energy=%g, p=%g, p/en=%g dB\n',i, ...
            en, m, m/en);
end

