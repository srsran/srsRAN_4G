function [ fs eps p_m w2] = find_pss( x, N_id_2, doplot, threshold)
    if nargin == 2
        doplot = false;
        threshold = 0;
    end
    if nargin == 3
        threshold = 0;
    end
    
    c=lte_pss_zc(N_id_2);
    cc=[zeros(33,1); c; zeros(33,1)];
    ccf=[0; cc(65:128); cc(2:64)];
    ccf=conj(ifft(ccf));

    w2=conv(x,ccf);
    if (doplot)
        plot(10*log10(abs(w2)./mean(abs(w2))));
        axis([0 length(w2) 0 20])
    end
    [m i]=max(abs(w2));
    fs=i-960;
    p_m = m/mean(abs(w2));
    
    if doplot
        fprintf('Frame starts at %d, m=%g, p=%g, p/m=%g dB\n',fs, ...
            mean(abs(w2)), m, 10*log10(m/mean(abs(w2))));
    end
    
    % Estimate PSS-aided CFO
    if (i > 200 && i<length(x)&& p_m > threshold)
        y=ccf.*x(i-128:i-1);
    
        y0=y(1:64);
        y1=y(65:length(y));    
    
        eps=angle(conj(sum(y0))*sum(y1))/pi;    
    else
         eps = NaN;
         fs = NaN;
    end
end

