function [symbs] = samps_to_symbs(samps, slot_start_idx, symb_offset, FFT_pad_size, scale)
    % Calculate index and CP length
    if(mod(symb_offset, 7) == 0)
        CP_len = 160;
    else
        CP_len = 144;
    end
    index = slot_start_idx + (2048+144)*symb_offset;
    if(symb_offset > 0)
        index = index + 16;
    end

    % Take FFT
    tmp = fftshift(fft(samps(index+CP_len:index+CP_len+2047)));

    % Remove DC subcarrier
    tmp_symbs = [tmp(FFT_pad_size+1:1024); tmp(1026:2048-(FFT_pad_size-1))];

    if(scale == 0)
        symbs = tmp_symbs;
    else
        for(n=1:length(tmp_symbs))
            symbs(n) = cos(angle(tmp_symbs(n))) + j*sin(angle(tmp_symbs(n)));
        end
    end
end