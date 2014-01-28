function [ fs eps p_m w2] = find_pss( x, N_id_2, doplot)
    if nargin == 2
        doplot = false;
    end
    
    c=lte_pss_zc(N_id_2);
    cc=[zeros(33,1); c; zeros(33,1)];
    ccf=[0; cc(65:128); cc(2:64)];
    ccf=conj(ifft(ccf));

    w2=conv(x,ccf);
    if (doplot)
        plot(abs(w2))
    end
    [m i]=max(abs(w2));
    fs=i-960;
    p_m = m/mean(abs(w2));
    if doplot
        fprintf('Frame starts at %d, m=%g, p=%g, p/m=%g dB\n',fs, ...
            mean(abs(w2)), m, 10*log10(m/mean(abs(w2))));
    end
    
    % Estimate PSS-aided CFO
%     if (i - 129)
%         y=ccf.*x(i-128:i-1);
%     
%         y0=y(1:64);
%         y1=y(65:length(y));    
%     
%         eps=angle(conj(sum(y0))*sum(y1))/pi;    
%     else
        eps = NaN;
%     end
end

