function [ fs eps p_m w2] = find_pss( x, N_id_2, fft_size)
    c=lte_pss_zc(N_id_2);
    cc=[zeros(fft_size/2-31,1); c; zeros(fft_size/2-31,1)];
    cc=[0; cc(fft_size/2+1:fft_size); cc(2:fft_size/2)];
    ccf=conj(ifft(cc));

    w2=conv(x,ccf);
        %plot(10*log10(abs(w2)));%./mean(abs(w2))));
   plot(abs(w2)) 
       %axis([0 length(w2) 0 20])
    [m i]=max(abs(w2));
    p_m = m/mean(abs(w2));
    
    fprintf('Frame starts at %d, m=%g, p=%g, p/m=%g dB\n',i, ...
            mean(abs(w2)), m, 10*log10(m/mean(abs(w2))));
        
end

