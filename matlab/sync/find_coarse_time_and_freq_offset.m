function [coarse_start freq_offset] = find_coarse_time_and_freq_offset(in, N_cp_l_else)

    % Decompose input
    in_re = real(in);
    in_im = imag(in);
    
    abs_corr = zeros(1,960);
    for(slot=0:10)
        for(n=1:40:960)
            corr_re = 0;
            corr_im = 0;
            for(z=1:N_cp_l_else)
                index   = (slot*960) + n-1 + z;
                corr_re = corr_re + in_re(index)*in_re(index+128) + in_im(index)*in_im(index+128);
                corr_im = corr_im + in_re(index)*in_im(index+128) - in_im(index)*in_re(index+128);
            end
            abs_corr(n) = abs_corr(n) + corr_re*corr_re + corr_im*corr_im;
        end
    end

    % Find first and second max
    abs_corr_idx = zeros(1,2);
    for(m=0:1)
        abs_corr_max = 0;
        for(n=1:480)
            if(abs_corr((m*480)+n) > abs_corr_max)
                abs_corr_max      = abs_corr((m*480)+n);
                abs_corr_idx(m+1) = (m*480)+n;
            end
        end
    end

    % Fine correlation and fraction frequency offset
    abs_corr      = zeros(1,960);
    corr_freq_err = zeros(1,960);
    for(slot=1:10)
        for(idx=1:2)
            if((abs_corr_idx(idx) - 40) < 1)
                abs_corr_idx(idx) = 41;
            end
            if((abs_corr_idx(idx) + 40) > 960)
                abs_corr_idx(idx) = 960 - 40;
            end
            for(n=abs_corr_idx(idx)-40:abs_corr_idx(idx)+40)
                corr_re = 0;
                corr_im = 0;
                for(z=1:N_cp_l_else)
                    index = (slot*960) + n-1 + z;
                    corr_re = corr_re + in_re(index)*in_re(index+128) + in_im(index)*in_im(index+128);
                    corr_im = corr_im + in_re(index)*in_im(index+128) - in_im(index)*in_re(index+128);
                end
                abs_corr(n)      = abs_corr(n) + corr_re*corr_re + corr_im*corr_im;
                corr_freq_err(n) = corr_freq_err(n) + atan2(corr_im, corr_re)/(128*2*pi*(0.0005/960));
            end
        end
    end

    % Find first and second max
    abs_corr_idx = zeros(1,2);
    for(m=0:1)
        abs_corr_max = 0;
        for(n=1:480)
            if(abs_corr((m*480)+n) > abs_corr_max)
                abs_corr_max      = abs_corr((m*480)+n);
                abs_corr_idx(m+1) = (m*480)+n;
            end
            end
        end

    % Determine frequency offset FIXME No integer offset is calculated here
    freq_offset = (corr_freq_err(abs_corr_idx(1))/10 + corr_freq_err(abs_corr_idx(2))/10)/2;23

    % Determine the symbol start locations from the correlation peaks
    % FIXME Needs some work
    tmp = abs_corr_idx(1);
    while(tmp > 0)
        tmp = tmp - 2192;
    end
    for(n=1:7)
        coarse_start(n) = tmp + (n*2192);
    end
end