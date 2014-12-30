%% PDSCH decoding based on RMC channels

%% Cell-Wide Settings
% A structure |enbConfig| is used to configure the eNodeB.
clear

Npackets = 10;
SNR_values = linspace(2,6,4);

%% Choose RMC 
[waveform,rgrid,rmccFgOut] = lteRMCDLTool('R.11',[1;0;0;1]);
waveform = sum(waveform,2);

Nsf = 8; 

%% Setup Fading channel model 
cfg.Seed = 8;                  % Random channel seed
cfg.NRxAnts = 1;               % 1 receive antenna
cfg.DelayProfile = 'EVA';      % EVA delay spread
cfg.DopplerFreq = 5;           % 120Hz Doppler frequency
cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
cfg.InitTime = 0;              % Initialize at time zero
cfg.NTerms = 16;               % Oscillators used in fading model
cfg.ModelType = 'GMEDS';       % Rayleigh fading model type
cfg.InitPhase = 'Random';      % Random initial phases
cfg.NormalizePathGains = 'On'; % Normalize delay profile power 
cfg.NormalizeTxAnts = 'On';    % Normalize for transmit antennas
cfg.SamplingRate = rmccFgOut.SamplingRate; 

% Setup channel equalizer
cec.PilotAverage = 'UserDefined';     % Type of pilot averaging
cec.FreqWindow = 9;                   % Frequency window size
cec.TimeWindow = 9;                   % Time window size
cec.InterpType = 'linear';             % 2D interpolation type
cec.InterpWindow = 'Centered';        % Interpolation window type
cec.InterpWinSize = 1;                % Interpolation window size

addpath('../../debug/lte/phy/lib/phch/test')

decoded = zeros(size(SNR_values));
decoded_liblte = zeros(size(SNR_values));

for snr_idx=1:length(SNR_values)
    SNRdB = SNR_values(snr_idx);
    SNR = 10^(SNRdB/10);    % Linear SNR  
    N0  = 1/(sqrt(2.0*rmccFgOut.CellRefP*double(rmccFgOut.Nfft))*SNR);
    for i=1:Npackets

        %% Fading
        rxWaveform = lteFadingChannel(cfg,waveform);
        
        %% Noise Addition
        noise = N0*complex(randn(size(rxWaveform)), randn(size(rxWaveform)));  % Generate noise
        rxWaveform = rxWaveform + noise; 
       
        %% Demodulate 
        frame_rx = lteOFDMDemodulate(rmccFgOut, rxWaveform);

        for sf_idx=0:Nsf
            flen=length(rxWaveform)/10;
            subframe_waveform = rxWaveform(sf_idx*flen+1:(sf_idx+1)*flen);
            subframe_rx=frame_rx(:,sf_idx*14+1:(sf_idx+1)*14);
            rmccFgOut.NSubframe=sf_idx;
            rmccFgOut.TotSubframes=1;

            % Perform channel estimation
            [hest, nest] = lteDLChannelEstimate(rmccFgOut, cec, subframe_rx);

            [cws,symbols] = ltePDSCHDecode(rmccFgOut,rmccFgOut.PDSCH,subframe_rx,hest,nest);
            [trblkout,blkcrc] = lteDLSCHDecode(rmccFgOut,rmccFgOut.PDSCH, ... 
                                                    rmccFgOut.PDSCH.TrBlkSizes(sf_idx+1),cws);

            decoded(snr_idx) = decoded(snr_idx) + ~blkcrc;


            %% Same with libLTE
            if (rmccFgOut.PDSCH.TrBlkSizes(sf_idx+1) > 0)
                [dec2, llr, pdschRx, pdschSymbols2] = liblte_pdsch(rmccFgOut, rmccFgOut.PDSCH, ... 
                                                        rmccFgOut.PDSCH.TrBlkSizes(sf_idx+1), ...
                                                        subframe_waveform);
            else
                dec2 = 1;
            end
            decoded_liblte(snr_idx) = decoded_liblte(snr_idx)+dec2;
        end
    end
    fprintf('SNR: %.1f\n',SNRdB)
end

if (length(SNR_values)>1)
    semilogy(SNR_values,1-decoded/Npackets/(Nsf+1),'bo-',...
             SNR_values,1-decoded_liblte/Npackets/(Nsf+1), 'ro-')
    grid on;
    legend('Matlab','libLTE')
    xlabel('SNR (dB)')
    ylabel('BLER')
    axis([min(SNR_values) max(SNR_values) 1/Npackets/(Nsf+1) 1])
else
    disp(decoded)
    disp(decoded_liblte)
end

