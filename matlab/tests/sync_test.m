clear
enb = lteTestModel('1.1','1.4MHz');
Ntrials = 1; 
SNR_values = 100;%linspace(-8,0,6);
flen=1920;
fft_size=128;
fading_enabled = true; 

tx_offset  = 10;%randi(50,1,Ntrials);

tx_signal = lteTestModelTool(enb); 
pss_idx = flen/2-fft_size+1:flen/2;
pss_signal=tx_signal(pss_idx);
tx_power = mean(pss_signal.*conj(pss_signal));

corrcfg.PSS='On';
corrcfg.SSS='On';
corrcfg.CellRS='Off';

cfg.Seed = 0;                  % Random channel seed
cfg.NRxAnts = 1;               % 1 receive antenna
cfg.DelayProfile = 'ETU';      % EVA delay spread
cfg.DopplerFreq = 120;         % 120Hz Doppler frequency
cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
cfg.InitTime = 0;              % Initialize at time zero
cfg.NTerms = 16;               % Oscillators used in fading model
cfg.ModelType = 'GMEDS';       % Rayleigh fading model type
cfg.InitPhase = 'Random';      % Random initial phases
cfg.NormalizePathGains = 'On'; % Normalize delay profile power 
cfg.NormalizeTxAnts = 'On';    % Normalize for transmit antennas
cfg.SamplingRate = flen*1000; 

addpath('../../build/srslte/lib/sync/test')
addpath('../sync/')

t = (0:length(tx_signal)-1).'/fft_size;

L=fft_size;
M=4;
diff=zeros(length(SNR_values),M);
for snr_idx=1:length(SNR_values)
    fprintf('SNR=%.1f dB\n', SNR_values(snr_idx));
    SNRdB = SNR_values(snr_idx);
    rx_offset = zeros(M,length(tx_offset));
    for i=1:Ntrials
        if fading_enabled
            [rx_signal, chinfo] = lteFadingChannel(cfg,tx_signal);
        else
            rx_signal = tx_signal; 
        end
        SNR = 10^(SNRdB/10);    % Linear SNR  
        rx = [zeros(tx_offset(i),1); rx_signal];
        N0  = tx_power/(sqrt(2.0)*SNR);
        noise = N0*complex(randn(size(rx)), randn(size(rx)));  % Generate noise
        rx=noise+rx;
        [rx_offset(1,i),corr_res] = lteDLFrameOffset(enb,rx(1:flen),corrcfg);
        
        % srsLTE in find mode
        [rx_offset(2,i),corr_lt] = srslte_pss(enb,rx(1:flen));
        rx_offset(2,i) = rx_offset(2,i) - flen/2;
    
        % srsLTE in tracking mode
        track_offset=2+rx_offset(2,i)+flen/2-fft_size-L/2;
        [rx_offset(3,i),corr_lt_track] = srslte_pss(enb,rx(track_offset:end),L);
        rx_offset(3,i) = rx_offset(2,i) + (rx_offset(3,i) - L/2) + 1;        
        
        % CP based
        [rx_offset(4,i), corr_lt_cp] = srslte_cp_synch(enb, rx);
        
    end
    if fading_enabled
        ch_delay = chinfo.ChannelFilterDelay;
    else
        ch_delay = 0; 
    end
    diff(snr_idx,:)=mean(abs(rx_offset-repmat(tx_offset,M,1) - ch_delay),2);
end

if (Ntrials == 1)
    disp(rx_offset)
    disp(diff)
    %plot(1:flen,abs(corr_res(1:flen)),1:flen,abs(corr_lt(1:flen)));
    t=1:L;
    plot(t,abs(corr_lt_track(t)));
    %plot(lambda)
    %plot(1:L,abs(corr_lt_track),[L/2, L/2], [0 max(abs(corr_lt_track))])
    grid on
    
else
    plot(SNR_values,diff);
    legend('Matlab','srs find','srs tracking','srs cp')
    grid on
    xlabel('SNR (dB)')
    ylabel('Avg time offset')
end

