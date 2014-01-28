function [ s1_m1 ] = compute_m1( m1, N_id_2)
%COMPUTE_S Summary of this function goes here
%   Detailed explanation goes here

    % Generate s_tilda
    x_s_tilda(0+1) = 0;
    x_s_tilda(1+1) = 0;
    x_s_tilda(2+1) = 0;
    x_s_tilda(3+1) = 0;
    x_s_tilda(4+1) = 1;
    for(i_hat=0:25)
        x_s_tilda(i_hat+5+1) = mod((x_s_tilda(i_hat+2+1) + x_s_tilda(i_hat+1)), 2);
    end
    for(idx=0:30)
        s_tilda(idx+1) = 1 - 2*x_s_tilda(idx+1);
    end

    % Generate c_tilda
    x_c_tilda(0+1) = 0;
    x_c_tilda(1+1) = 0;
    x_c_tilda(2+1) = 0;
    x_c_tilda(3+1) = 0;
    x_c_tilda(4+1) = 1;
    for(i_hat=0:25)
        x_c_tilda(i_hat+5+1) = mod((x_c_tilda(i_hat+3+1) + x_c_tilda(i_hat+1)), 2);
    end
    for(idx=0:30)
        c_tilda(idx+1) = 1 - 2*x_c_tilda(idx+1);
    end

    % Generate z_tilda
    x_z_tilda(0+1) = 0;
    x_z_tilda(1+1) = 0;
    x_z_tilda(2+1) = 0;
    x_z_tilda(3+1) = 0;
    x_z_tilda(4+1) = 1;
    for(i_hat=0:25)
        x_z_tilda(i_hat+5+1) = mod((x_z_tilda(i_hat+4+1) + x_z_tilda(i_hat+2+1) + x_z_tilda(i_hat+1+1) + x_z_tilda(i_hat+1)), 2);
    end
    for(idx=0:30)
        z_tilda(idx+1) = 1 - 2*x_z_tilda(idx+1);
    end

    % Generate s0_m0 and s1_m1
    for(n=0:30)
        s1_m1(n+1) = s_tilda(mod(n + m1, 31)+1);
    end
end

