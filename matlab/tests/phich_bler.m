clear

%% PHICH encoding/decoding

%% Setup simulation
Npackets = 80;
SNR_values = linspace(-6,0,6);
enable_fading=false; 
addpath('../../debug/srslte/lib/phch/test')


%% Cell-Wide Settings
enbConfig.NDLRB = 50;                % No of Downlink RBs in total BW
enbConfig.CyclicPrefix = 'Normal';  % CP length
enbConfig.Ng = 'One';             % HICH groups
enbConfig.CellRefP = 1;             % 1-antenna ports
enbConfig.NCellID = 36;             % Physical layer cell identity
enbConfig.NSubframe = 5;            % Subframe number 0
enbConfig.DuplexMode = 'FDD';       % Frame structure
enbConfig.PHICHDuration = 'Normal';

%% Define HI resource 
hi_res = [1 1]; 
ack_bit = 1; 

%% Setup Fading channel model 
if (enable_fading)
    cfg.Seed = 8;                  % Random channel seed
    cfg.NRxAnts = 1;               % 1 receive antenna
    cfg.DelayProfile = 'EPA';      % EVA delay spread
    cfg.DopplerFreq = 5;           % 120Hz Doppler frequency
    cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
    cfg.InitTime = 0;              % Initialize at time zero
    cfg.NTerms = 16;               % Oscillators used in fading model
    cfg.ModelType = 'GMEDS';       % Rayleigh fading model type
    cfg.InitPhase = 'Random';      % Random initial phases
    cfg.NormalizePathGains = 'On'; % Normalize delay profile power 
    cfg.NormalizeTxAnts = 'On';    % Normalize for transmit antennas
end

% Setup channel equalizer
cec.PilotAverage = 'UserDefined';     % Type of pilot averaging
cec.FreqWindow = 9;                   % Frequency window size
cec.TimeWindow = 9;                   % Time window size
cec.InterpType = 'linear';             % 2D interpolation type
cec.InterpWindow = 'Causal';        % Interpolation window type
cec.InterpWinSize = 1;                % Interpolation window size

%% Generate TX subframe
subframe_tx = lteDLResourceGrid(enbConfig);

%% Genearte PHICH signal
hi_sym_tx  = ltePHICH(enbConfig, [hi_res ack_bit]);
hi_indices = ltePHICHIndices(enbConfig); 
subframe_tx(hi_indices)=hi_sym_tx; 

%% Add references to subframe 
cellRsSym = lteCellRS(enbConfig);
cellRsInd = lteCellRSIndices(enbConfig);
subframe_tx(cellRsInd) = cellRsSym;

%% Modulate signal 
[txWaveform, info] = lteOFDMModulate(enbConfig,subframe_tx);
cfg.SamplingRate = info.SamplingRate;

%% Start simulation
decoded = zeros(size(SNR_values));
decoded_srslte = zeros(size(SNR_values));

for snr_idx=1:length(SNR_values)
    SNRdB = SNR_values(snr_idx);
    SNR = 10^(SNRdB/10);    % Linear SNR  
    N0  = 1/(sqrt(2.0*enbConfig.CellRefP*double(info.Nfft))*SNR);
    for i=1:Npackets
        
        %% Fading
        rxWaveform = sum(txWaveform,2);
        if (enable_fading)
            rxWaveform = lteFadingChannel(cfg,rxWaveform);
        end
        
        %% Noise Addition
        noise = N0*complex(randn(size(rxWaveform)), randn(size(rxWaveform)));  
        rxWaveform = rxWaveform + noise; 
       
        %% Demodulate 
        subframe_rx = lteOFDMDemodulate(enbConfig, rxWaveform);

        %% Channel estimation
        if (enable_fading)
            [hest, nest] = lteDLChannelEstimate(enbConfig, cec, subframe_rx);
        else
            hest=ones(size(subframe_rx));
            nest=0;
        end

        %% Extract resources
        phichSymbolsRx   = subframe_rx(hi_indices);
        phichSymbolsHest = hest(hi_indices);

        %% PHICH decoding
        [hi, hi_symbols] = ltePHICHDecode(enbConfig,hi_res, phichSymbolsRx, phichSymbolsHest, nest);        
        decoded(snr_idx) = decoded(snr_idx) + (hi == ack_bit); 
        
        %% Same with srsLTE
        [hi_srslte, hi_symbols_srslte] = srslte_phich(enbConfig, hi_res, subframe_rx, hest, nest);
        decoded_srslte(snr_idx) = decoded_srslte(snr_idx) + (hi_srslte == ack_bit); 
    end
    fprintf('SNR: %.1f\n',SNRdB)
end

if (Npackets>1)
    semilogy(SNR_values,1-decoded/Npackets,'bo-',...
             SNR_values,1-decoded_srslte/Npackets, 'ro-')
    grid on
    legend('Matlab','srsLTE')
    xlabel('SNR (dB)')
    ylabel('BLER')
    axis([min(SNR_values) max(SNR_values) 1/Npackets/10 1])
else
    
    scatter(real(hi_symbols),imag(hi_symbols))
    hold on
    scatter(real(hi_symbols_srslte),imag(hi_symbols_srslte))
    hold off
    grid on;
    axis([-2 2 -2 2])
    disp(hi)
    disp(hi_srslte)
    
end

