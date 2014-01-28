
function [c] = lte_generate_prs_c(c_init, seq_len)
    % Initialize the m-sequences
    x1  = zeros(1,1600+seq_len);
    x2  = zeros(1,1600+seq_len);
    tmp = c_init;
    for(n=0:30)
        x2(30-n+1) = floor(tmp/(2^(30-n)));
        tmp        = tmp - (floor(tmp/(2^(30-n)))*2^(30-n));
    end
    x1(0+1) = 1;

    % Advance m-sequences
    for(n=0:1600+seq_len)
        x1(n+31+1) = mod(x1(n+3+1) + x1(n+1), 2);
        x2(n+31+1) = mod(x2(n+3+1) + x2(n+2+1) + x2(n+1+1) + x2(n+1), 2);
    end

    % Generate c
    for(n=0:seq_len-1)
        c(n+1) = mod(x1(n+1600+1) + x2(n+1600+1), 2);
    end
end
