%
% Copyright 2011-2012 Ben Wojtowicz
%
%    This program is free software: you can redistribute it and/or modify
%    it under the terms of the GNU Affero General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    This program is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU Affero General Public License for more details.
%
%    You should have received a copy of the GNU Affero General Public License
%    along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
% Function:    lte_generate_sss
% Description: Generates LTE secondary synchronization signals
% Inputs:      N_id_1    - Physical layer cell identity group
%              N_id_2    - Physical layer identity
% Outputs:     sss_d_u_0 - The sequence d(n) used for the secondary
%                          synchronization signal, an interleaved
%                          concatenation of two length-31 binary
%                          sequences for subframe 0
% Outputs:     sss_d_u_5 - The sequence d(n) used for the secondary
%                          synchronization signal, an interleaved
%                          concatenation of two length-31 binary
%                          sequences for subframe 5
% Spec:        3GPP TS 36.211 section 6.11.2.1 v10.1.0
% Notes:       None
% Rev History: Ben Wojtowicz 10/28/2011 Created
%              Ben Wojtowicz 01/29/2012 Fixed license statement
%
function [sss_d_u_0, sss_d_u_5 c0 c1 m0 m1] = lte_generate_sss(N_id_1, N_id_2)
    % Validate N_id_1
    if(~(N_id_1 >= 0 && N_id_1 <= 167))
        fprintf('ERROR: Invalid N_id_1 (%u)\n', N_id_1);
        sss_d_u_0 = 0;
        sss_d_u_5 = 0;
        return;
    end

    % Validate N_id_2
    if(~(N_id_2 >= 0 && N_id_2 <= 2))
        fprintf('ERROR: Invalid N_id_2 (%u)\n', N_id_2);
        sss_d_u_0 = 0;
        sss_d_u_5 = 0;
        return;
    end

    % Generate m0 and m1
    q_prime = floor(N_id_1/30);
    q       = floor((N_id_1 + (q_prime*(q_prime+1)/2))/30);
    m_prime = N_id_1 + (q*(q+1)/2);
    m0      = mod(m_prime, 31);
    m1      = mod((m0 + floor(m_prime/31) + 1), 31);

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
        s0_m0(n+1) = s_tilda(mod(n + m0, 31)+1);
        s1_m1(n+1) = s_tilda(mod(n + m1, 31)+1);
    end

    % Generate c0 and c1
    for(n=0:30)
        c0(n+1) = c_tilda(mod(n + N_id_2, 31)+1);
        c1(n+1) = c_tilda(mod(n + N_id_2 + 3, 31)+1);
    end

    % Generate z1_m0 and z1_m1
    for(n=0:30)
        z1_m0(n+1) = z_tilda(mod(n + mod(m0, 8), 31)+1);
        z1_m1(n+1) = z_tilda(mod(n + mod(m1, 8), 31)+1);
    end

    % Generate SSS
    for(n=0:30)
        sss_d_u_0(2*n+1)   = s0_m0(n+1) * c0(n+1);
        sss_d_u_5(2*n+1)   = s1_m1(n+1) * c0(n+1);

        sss_d_u_0(2*n+1+1) = s1_m1(n+1) * c1(n+1) * z1_m0(n+1);
        sss_d_u_5(2*n+1+1) = s0_m0(n+1) * c1(n+1) * z1_m1(n+1);
    end
end
