function [ symb, ce ] = get_ce( samps, N_sf, N_id_cell, N_ant )

    N_sc_rb      = 12; % Only dealing with normal cp at this time
    N_rb_dl_max  = 110;
    v_shift      = mod(N_id_cell, 6);
    sf_start_idx = f_start_idx + N_sf*30720;
    crs0         = lte_generate_crs(mod(N_sf*2+0, 20), 0, N_id_cell);
    crs1         = lte_generate_crs(mod(N_sf*2+0, 20), 1, N_id_cell);
    crs4         = lte_generate_crs(mod(N_sf*2+0, 20), 4, N_id_cell);
    crs7         = lte_generate_crs(mod(N_sf*2+1, 20), 0, N_id_cell);
    crs8         = lte_generate_crs(mod(N_sf*2+1, 20), 1, N_id_cell);
    crs11        = lte_generate_crs(mod(N_sf*2+1, 20), 4, N_id_cell);
    crs14        = lte_generate_crs(mod(N_sf*2+2, 20), 0, N_id_cell);
    crs15        = lte_generate_crs(mod(N_sf*2+2, 20), 1, N_id_cell);

    N_rb_dl      = 6;
    FFT_pad_size = 988; % FFT_size = 2048
     
    for(n=0:15)
        if(n < 7)
            idx = sf_start_idx;
        elseif(n < 14)
            idx = sf_start_idx + 15360;
        else
            idx = sf_start_idx + 2*15360;
        end
        symb(n+1,:) = samps_to_symbs(samps, idx, mod(n,7), FFT_pad_size, 0);
    end
    
    for(p=0:N_ant-1)
        % Define v, crs, sym, and N_sym
        if(p == 0)
            v     = [0, 3, 0, 3, 0];
            crs   = [crs0; crs4; crs7; crs11; crs14];
            sym   = [symb(0+1,:); symb(4+1,:); symb(7+1,:); symb(11+1,:); symb(14+1,:)];
            N_sym = 5;
        elseif(p == 1)
            v     = [3, 0, 3, 0, 3];
            crs   = [crs0; crs4; crs7; crs11; crs14];
            sym   = [symb(0+1,:); symb(4+1,:); symb(7+1,:); symb(11+1,:); symb(14+1,:)];
            N_sym = 5;
        elseif(p == 2)
            v     = [0, 3, 0];
            crs   = [crs1; crs8; crs15];
            sym   = [symb(1+1,:); symb(8+1,:); symb(15+1,:)];
            N_sym = 3;
        else % p == 3
            v     = [3, 6, 3];
            crs   = [crs1; crs8; crs15];
            sym   = [symb(1+1,:); symb(8+1,:); symb(15+1,:)];
            N_sym = 3;
        end

        for(n=1:N_sym)
            for(m=0:2*N_rb_dl-1)
                k          = 6*m + mod((v(n) + v_shift), 6);
                m_prime    = m + N_rb_dl_max - N_rb_dl;
                tmp        = sym(n,k+1)/crs(n,m_prime+1);
                mag(n,k+1) = abs(tmp);
                ang(n,k+1) = angle(tmp);

                % Unwrap phase
                if(m > 0)
                    while((ang(n,k+1) - ang(n,k-6+1)) > pi)
                        ang(n,k+1) = ang(n,k+1) - 2*pi;
                    end
                    while((ang(n,k+1) - ang(n,k-6+1)) < -pi)
                        ang(n,k+1) = ang(n,k+1) + 2*pi;
                    end
                end

                % Interpolate between CRSs (simple linear interpolation)
                if(m > 0)
                    frac_mag = (mag(n,k+1) - mag(n,k-6+1))/6;
                    frac_ang = (ang(n,k+1) - ang(n,k-6+1))/6;
                    for(o=1:5)
                        mag(n,k-o+1) = mag(n,k-(o-1)+1) - frac_mag;
                        ang(n,k-o+1) = ang(n,k-(o-1)+1) - frac_ang;
                    end
                end

                % Interpolate before 1st CRS
                if(m == 1)
                    for(o=1:mod(v(n) + v_shift, 6))
                        mag(n,k-6-o+1) = mag(n,k-6-(o-1)+1) - frac_mag;
                        ang(n,k-6-o+1) = ang(n,k-6-(o-1)+1) - frac_ang;
                    end
                end
            end

            % Interpolate after last CRS
            for(o=1:(5-mod(v(n) + v_shift, 6)))
                mag(n,k+o+1) = mag(n,k+(o-1)+1) - frac_mag;
                ang(n,k+o+1) = ang(n,k+(o-1)+1) - frac_ang;
            end
        end

        % Interpolate between symbols and construct channel estimates
        if(N_sym == 3)
            for(n=1:N_sc_rb*N_rb_dl)
                % Construct symbol 1 and 8 channel estimates directly
                ce(p+1,1+1,n) = mag(1,n)*(cos(ang(1,n)) + j*sin(ang(1,n)));
                ce(p+1,8+1,n) = mag(2,n)*(cos(ang(2,n)) + j*sin(ang(2,n)));

                % Interpolate for symbol 2, 3, 4, 5, 6, and 7 channel estimates
                frac_mag = (mag(2,n) - mag(1,n))/7;
                frac_ang = ang(2,n) - ang(1,n);
                if(frac_ang >= pi) % Wrap angle
                    frac_ang = frac_ang - 2*pi;
                elseif(frac_ang <= -pi)
                    frac_ang = frac_ang + 2*pi;
                end
                frac_ang = frac_ang/7;
                ce_mag   = mag(2,n);
                ce_ang   = ang(2,n);
                for(o=7:-1:2)
                    ce_mag        = ce_mag - frac_mag;
                    ce_ang        = ce_ang - frac_ang;
                    ce(p+1,o+1,n) = ce_mag*(cos(ce_ang) + j*sin(ce_ang));
                end

                % Interpolate for symbol 0 channel estimate
                % FIXME: Use previous slot to do this correctly
                ce_mag        = mag(1,n) - frac_mag;
                ce_ang        = ang(1,n) - frac_ang;
                ce(p+1,0+1,n) = ce_mag*(cos(ce_ang) + j*sin(ce_ang));

                % Interpolate for symbol 9, 10, 11, 12, and 13 channel estimates
                frac_mag = (mag(3,n) - mag(2,n))/7;
                frac_ang = ang(3,n) - ang(2,n);
                if(frac_ang >= pi) % Wrap angle
                    frac_ang = frac_ang - 2*pi;
                elseif(frac_ang <= -pi)
                    frac_ang = frac_ang + 2*pi;
                end
                frac_ang = frac_ang/7;
                ce_mag   = mag(3,n) - frac_mag;
                ce_ang   = ang(3,n) - frac_ang;
                for(o=13:-1:9)
                    ce_mag        = ce_mag - frac_mag;
                    ce_ang        = ce_ang - frac_ang;
                    ce(p+1,o+1,n) = ce_mag*(cos(ce_ang) + j*sin(ce_ang));
                end
            end
        else
            for(n=1:N_sc_rb*N_rb_dl)
                % Construct symbol 0, 4, 7, and 11 channel estimates directly
                ce(p+1,0+1,n)  = mag(1,n)*(cos(ang(1,n)) + j*sin(ang(1,n)));
                ce(p+1,4+1,n)  = mag(2,n)*(cos(ang(2,n)) + j*sin(ang(2,n)));
                ce(p+1,7+1,n)  = mag(3,n)*(cos(ang(3,n)) + j*sin(ang(3,n)));
                ce(p+1,11+1,n) = mag(4,n)*(cos(ang(4,n)) + j*sin(ang(4,n)));

                % Interpolate for symbol 1, 2, and 3 channel estimates
                frac_mag = (mag(2,n) - mag(1,n))/4;
                frac_ang = ang(2,n) - ang(1,n);
                if(frac_ang >= pi) % Wrap angle
                    frac_ang = frac_ang - 2*pi;
                elseif(frac_ang <= -pi)
                    frac_ang = frac_ang + 2*pi;
                end
                frac_ang = frac_ang/4;
                ce_mag   = mag(2,n);
                ce_ang   = ang(2,n);
                for(o=3:-1:1)
                    ce_mag        = ce_mag - frac_mag;
                    ce_ang        = ce_ang - frac_ang;
                    ce(p+1,o+1,n) = ce_mag*(cos(ce_ang) + j*sin(ce_ang));
                end

                % Interpolate for symbol 5 and 6 channel estimates
                frac_mag = (mag(3,n) - mag(2,n))/3;
                frac_ang = ang(3,n) - ang(2,n);
                if(frac_ang >= pi) % Wrap angle
                    frac_ang = frac_ang - 2*pi;
                elseif(frac_ang <= -pi)
                    frac_ang = frac_ang + 2*pi;
                end
                frac_ang = frac_ang/3;
                ce_mag   = mag(3,n);
                ce_ang   = ang(3,n);
                for(o=6:-1:5)
                    ce_mag        = ce_mag - frac_mag;
                    ce_ang        = ce_ang - frac_ang;
                    ce(p+1,o+1,n) = ce_mag*(cos(ce_ang) + j*sin(ce_ang));
                end

                % Interpolate for symbol 8, 9, and 10 channel estimates
                frac_mag = (mag(4,n) - mag(3,n))/4;
                frac_ang = ang(4,n) - ang(3,n);
                if(frac_ang >= pi) % Wrap angle
                    frac_ang = frac_ang - 2*pi;
                elseif(frac_ang <= -pi)
                    frac_ang = frac_ang + 2*pi;
                end
                frac_ang = frac_ang/4;
                ce_mag   = mag(4,n);
                ce_ang   = ang(4,n);
                for(o=10:-1:8)
                    ce_mag        = ce_mag - frac_mag;
                    ce_ang        = ce_ang - frac_ang;
                    ce(p+1,o+1,n) = ce_mag*(cos(ce_ang) + j*sin(ce_ang));
                end

                % Interpolate for symbol 12 and 13 channel estimates
                frac_mag = (mag(5,n) - mag(4,n))/3;
                frac_ang = ang(5,n) - ang(4,n);
                if(frac_ang >= pi) % Wrap angle
                    frac_ang = frac_ang - 2*pi;
                elseif(frac_ang <= -pi)
                    frac_ang = frac_ang + 2*pi;
                end
               
            
                frac_ang = frac_ang/3;
                ce_mag   = mag(5,n);
                ce_ang   = ang(5,n);
                for(o=13:-1:12)
                    ce_mag        = ce_mag - frac_mag;
                    ce_ang        = ce_ang - frac_ang;
                    ce(p+1,o+1,n) = ce_mag*(cos(ce_ang) + j*sin(ce_ang));
                end                
            end
        end
    end
    subplot(1,2,1)
    pcolor(transpose(abs(reshape(ce(1,:,:),14,[]))))
    subplot(1,2,2)
    pcolor(transpose(real(symb(:,:))))
end

