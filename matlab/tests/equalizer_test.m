%% LTE Downlink Channel Estimation and Equalization

%% Cell-Wide Settings

clear

SNR_values_db=15;%linspace(5,20,8);
Nrealizations=1;

preEVM = zeros(length(SNR_values_db),Nrealizations);
postEVM_mmse = zeros(length(SNR_values_db),Nrealizations);
postEVM_mmse2 = zeros(length(SNR_values_db),Nrealizations);
postEVM_zf = zeros(length(SNR_values_db),Nrealizations);
postEVM_zf2 = zeros(length(SNR_values_db),Nrealizations);


enb.NDLRB = 6;                 % Number of resource blocks
enb.CellRefP = 1;               % One transmit antenna port
enb.NCellID = 0;               % Cell ID
enb.CyclicPrefix = 'Normal';    % Normal cyclic prefix
enb.DuplexMode = 'FDD';         % FDD

%% Channel Model Configuration
rng(1);         % Configure random number generators

cfg.Seed = 1;                  % Random channel seed
cfg.NRxAnts = 1;               % 1 receive antenna
cfg.DelayProfile = 'EVA';      % EVA delay spread
cfg.DopplerFreq = 120;         % 120Hz Doppler frequency
cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
cfg.InitTime = 0;              % Initialize at time zero
cfg.NTerms = 16;               % Oscillators used in fading model
cfg.ModelType = 'GMEDS';       % Rayleigh fading model type
cfg.InitPhase = 'Random';      % Random initial phases
cfg.NormalizePathGains = 'On'; % Normalize delay profile power 
cfg.NormalizeTxAnts = 'On';    % Normalize for transmit antennas

%% Channel Estimator Configuration

cec.FreqWindow = 9;               % Frequency averaging window in 
                                  % Resource Elements (REs)
cec.TimeWindow = 9;               % Time averaging window in REs
cec.InterpType = 'Cubic';         % Cubic interpolation
cec.PilotAverage = 'UserDefined'; % Pilot averaging method
cec.InterpWinSize = 3;            % Interpolate up to 3 subframes 
                                  % simultaneously
cec.InterpWindow = 'Centred';     % Interpolation windowing method

cec2.FreqWindow = 9;               % Frequency averaging window in 
                                  % Resource Elements (REs)
cec2.TimeWindow = 9;               % Time averaging window in REs
cec2.InterpType = 'Linear';         % Cubic interpolation
cec2.PilotAverage = 'UserDefined'; % Pilot averaging method
cec2.InterpWinSize = 3;            % Interpolate up to 3 subframes 
                                  % simultaneously
cec2.InterpWindow = 'Centered';     % Interpolation windowing method

%% Subframe Resource Grid Size

gridsize = lteDLResourceGridSize(enb);
K = gridsize(1);    % Number of subcarriers
L = gridsize(2);    % Number of OFDM symbols in one subframe
P = gridsize(3);    % Number of transmit antenna ports

for nreal=1:Nrealizations
%% Transmit Resource Grid
txGrid = [];

%% Payload Data Generation
% Number of bits needed is size of resource grid (K*L*P) * number of bits
% per symbol (2 for QPSK)
numberOfBits = K*L*P*2; 

% Create random bit stream
inputBits = randi([0 1], numberOfBits, 1); 

% Modulate input bits
inputSym = lteSymbolModulate(inputBits,'QPSK');	

%% Frame Generation

% For all subframes within the frame
for sf = 0:10
    
    % Set subframe number
    enb.NSubframe = mod(sf,10);
    
    % Generate empty subframe
    subframe = lteDLResourceGrid(enb);
    
    % Map input symbols to grid
    subframe(:) = inputSym;

    % Generate synchronizing signals
    pssSym = ltePSS(enb);
    sssSym = lteSSS(enb);
    pssInd = ltePSSIndices(enb);
    sssInd = lteSSSIndices(enb);

    % Map synchronizing signals to the grid
    subframe(pssInd) = pssSym;
    subframe(sssInd) = sssSym;

    % Generate cell specific reference signal symbols and indices
    cellRsSym = lteCellRS(enb);
    cellRsInd = lteCellRSIndices(enb);

    % Map cell specific reference signal to grid
    subframe(cellRsInd) = cellRsSym;
    
    % Append subframe to grid to be transmitted
    txGrid = [txGrid subframe]; %#ok
      
end

%% OFDM Modulation

[txWaveform,info] = lteOFDMModulate(enb,txGrid); 
txGrid = txGrid(:,1:140);

%% SNR Configuration

for snr_idx=1:length(SNR_values_db)
SNRdB = SNR_values_db(snr_idx);             % Desired SNR in dB
SNR = 10^(SNRdB/20);    % Linear SNR  


%% Fading Channel

cfg.SamplingRate = info.SamplingRate;

% Pass data through the fading channel model
rxWaveform = lteFadingChannel(cfg,txWaveform);

%% Additive Noise

% Calculate noise gain
N0 = 1/(sqrt(2.0*enb.CellRefP*double(info.Nfft))*SNR);

% Create additive white Gaussian noise
noise = N0*complex(randn(size(rxWaveform)),randn(size(rxWaveform)));   

% Add noise to the received time domain waveform
%rxWaveform = rxWaveform + noise;

%% Synchronization

offset = lteDLFrameOffset(enb,rxWaveform);
rxWaveform = rxWaveform(1+offset:end,:);

%% OFDM Demodulation
rxGrid = lteOFDMDemodulate(enb,rxWaveform);
%rxGrid = txGrid;

addpath('../../debug/lte/phy/lib/ch_estimation/test')
%% Channel Estimation
[estChannel, noiseEst, avg_ref1, noavg_ref1] = lteDLChannelEstimate2(enb,cec2,rxGrid);
[dumm, refs] = liblte_chest(enb.NCellID,enb.CellRefP,rxGrid);
%estChannel2=reshape(estChannel2,72,[]);
[estChannel2] = lteDLChannelEstimate3(enb,cec2,rxGrid,refs);

%error(snr_idx,nreal) = mean(mean(abs(avg_ref-transpose(refs)),2));

%% MMSE Equalization
eqGrid_mmse = lteEqualizeMMSE(rxGrid, estChannel, noiseEst);
eqGrid_mmse2 = lteEqualizeMMSE(rxGrid, estChannel2, noiseEst);

eqGrid_zf = lteEqualizeZF(rxGrid, estChannel);
eqGrid_zf2 = lteEqualizeZF(rxGrid, estChannel2);

%% Analysis

% Compute EVM across all input values
% EVM of pre-equalized receive signal
preEqualisedEVM = lteEVM(txGrid,rxGrid);
fprintf('%d-%d: Pre-EQ: %0.3f%%\n', ...
        snr_idx,nreal,preEqualisedEVM.RMS*100); 
    
%EVM of post-equalized receive signal
postEqualisedEVM_mmse = lteEVM(txGrid,eqGrid_mmse);
fprintf('%d-%d: MMSE: %0.3f%%\n', ...
        snr_idx,nreal,postEqualisedEVM_mmse.RMS*100); 
postEqualisedEVM_mmse2 = lteEVM(txGrid,eqGrid_mmse2);
fprintf('%d-%d: MMSE-lin: %0.3f%%\n', ...
        snr_idx,nreal,postEqualisedEVM_mmse2.RMS*100); 

    
postEqualisedEVM_zf = lteEVM(txGrid,eqGrid_zf);
fprintf('%d-%d: zf: %0.3f%%\n', ...
        snr_idx,nreal,postEqualisedEVM_zf.RMS*100); 
postEqualisedEVM_zf2 = lteEVM(txGrid,eqGrid_zf2);
fprintf('%d-%d: zf-linear: %0.3f%%\n', ...
        snr_idx,nreal,postEqualisedEVM_zf2.RMS*100); 

preEVM(snr_idx,nreal) =preEqualisedEVM.RMS;
postEVM_mmse(snr_idx,nreal) = postEqualisedEVM_mmse.RMS;
postEVM_mmse2(snr_idx,nreal) = postEqualisedEVM_mmse2.RMS;
postEVM_zf(snr_idx,nreal) = postEqualisedEVM_zf.RMS;
postEVM_zf2(snr_idx,nreal) = postEqualisedEVM_zf2.RMS;
    
end
end


plot(SNR_values_db, mean(preEVM,2), ...
    SNR_values_db, mean(postEVM_mmse,2), ...
    SNR_values_db, mean(postEVM_mmse2,2), ...
    SNR_values_db, mean(postEVM_zf,2), ...
    SNR_values_db, mean(postEVM_zf2,2))
legend('No Eq','MMSE','MMSE-linear','ZF','ZF-linear')
%plot(SNR_values_db, mean(error,2))
grid on

