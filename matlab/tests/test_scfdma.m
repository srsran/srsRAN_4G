clear
ue = struct('NULRB',6,'NDLRB',6);
d = lteULResourceGridSize(ue);
rgrid = complex(rand(d)-0.5,rand(d)-0.5);
waveform = lteSCFDMAModulate(ue,rgrid,0);

wave = [];
freq_correct=[];
for i=1:14
    symbol = ifft(ifftshift([zeros(28,1); rgrid(:,i); zeros(28,1)]),128);
    if (i==1 || i==8)
        cp_len=10;
    else
        cp_len=9;
    end
    symbol_cp=[symbol((128-cp_len+1):128); symbol(1:128)];
    freq_correct=[freq_correct; exp(1i*pi*(-cp_len:127)'/128)];    
    wave = [wave; symbol_cp];
end

wave=wave.*freq_correct;

%wave=transpose(wave);
%plot(1:138,fftshift(fft(waveform(1:138))),1:138,fftshift(fft(wave(1:138))))
plot(abs(wave(1:138)-waveform(1:138)))
mean(abs(wave-waveform))
