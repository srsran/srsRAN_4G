function [r] = lte_generate_crs(ns, l, cell_id)

    % Calculate c_init and sequence length
    N_cp        = 1;
    c_init      = 1024 * (7 * (ns+1) + l + 1) * (2 * cell_id + 1) + 2*cell_id + N_cp;
    
    % Generate the psuedo random sequence c
    c = lte_generate_prs_c(c_init, 220);

    % Construct r
    r = zeros(1,len);
    for(m=0:len-1)
        r(m+1) = (1/sqrt(2))*(1 - 2*c(2*m+1)) + j*(1/sqrt(2))*(1 - 2*c(2*m+1+1));
    end
end
