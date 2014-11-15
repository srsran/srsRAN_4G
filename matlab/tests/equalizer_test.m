%% LTE Downlink Channel Estimation and Equalization

%% Cell-Wide Settings

clear

SNR_values_db=[0 1 2];%15;%[5 10 15];%linspace(0,20,8);
Nrealizations=1;

preEVM = zeros(length(SNR_values_db),Nrealizations);
postEVM_mmse = zeros(length(SNR_values_db),Nrealizations);
postEVM_mmse_lin = zeros(length(SNR_values_db),Nrealizations);
postEVM_liblte = zeros(length(SNR_values_db),Nrealizations);


enb.NDLRB = 6;                 % Number of resource blocks
enb.CellRefP = 1;               % One transmit antenna port
enb.NCellID = 0;               % Cell ID
enb.CyclicPrefix = 'Normal';    % Normal cyclic prefix
enb.DuplexMode = 'FDD';         % FDD

%% Channel Model Configuration
rng(1);         % Configure random number generators

cfg.Seed = 2;                  % Random channel seed
cfg.NRxAnts = 1;               % 1 receive antenna
cfg.DelayProfile = 'EVA';      % EVA delay spread
cfg.DopplerFreq = 5;         % 120Hz Doppler frequency
cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
cfg.InitTime = 0;              % Initialize at time zero
cfg.NTerms = 16;               % Oscillators used in fading model
cfg.ModelType = 'GMEDS';       % Rayleigh fading model type
cfg.InitPhase = 'Random';      % Random initial phases
cfg.NormalizePathGains = 'On'; % Normalize delay profile power 
cfg.NormalizeTxAnts = 'On';    % Normalize for transmit antennas

%% Channel Estimator Configuration
cec = struct;                        % Channel estimation config structure
cec.PilotAverage = 'UserDefined';    % Type of pilot symbol averaging
cec.FreqWindow = 9;                 % Frequency window size
cec.TimeWindow = 9;                 % Time window size
cec.InterpType = 'Linear';            % 2D interpolation type
cec.InterpWindow = 'Centered';       % Interpolation window type
cec.InterpWinSize = 1;               % Interpolation window size

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
rxWaveform = rxWaveform + noise;

%% Synchronization

offset = lteDLFrameOffset(enb,rxWaveform);
rxWaveform = rxWaveform(1+offset:end,:);

%% OFDM Demodulation
rxGrid = lteOFDMDemodulate(enb,rxWaveform);

addpath('../../debug/lte/phy/lib/ch_estimation/test')

%% Channel Estimation
[estChannel, noiseEst] = lteDLChannelEstimate(enb,cec,rxGrid);
output=[];
for i=0:9
    [d, a, out] = liblte_chest(enb.NCellID,enb.CellRefP,rxGrid(:,i*14+1:(i+1)*14),[0.15 0.7 0.15],[0.1 0.9],i);
    output = [output out];
end
%% MMSE Equalization
eqGrid_mmse = lteEqualizeMMSE(rxGrid, estChannel, noiseEst);

eqGrid_liblte = reshape(output,size(eqGrid_mmse));

%% Analysis

% Compute EVM across all input values
% EVM of pre-equalized receive signal
preEqualisedEVM = lteEVM(txGrid,rxGrid);
fprintf('%d-%d: Pre-EQ: %0.3f%%\n', ...
        snr_idx,nreal,preEqualisedEVM.RMS*100); 


%EVM of post-equalized receive signal
postEqualisedEVM_mmse = lteEVM(txGrid,reshape(eqGrid_mmse,size(txGrid)));
fprintf('%d-%d: MMSE: %0.3f%%\n', ...
        snr_idx,nreal,postEqualisedEVM_mmse.RMS*100); 

postEqualisedEVM_liblte = lteEVM(txGrid,reshape(eqGrid_liblte,size(txGrid)));
fprintf('%d-%d: liblte: %0.3f%%\n', ...
        snr_idx,nreal,postEqualisedEVM_liblte.RMS*100); 

preEVM(snr_idx,nreal) = preEqualisedEVM.RMS;
postEVM_mmse(snr_idx,nreal) = mean([postEqualisedEVM_mmse.RMS]);
postEVM_liblte(snr_idx,nreal) = mean([postEqualisedEVM_liblte.RMS]);
end
end

% plot(SNR_values_db,20*log10(1/sqrt(2.0*enb.CellRefP*double(info.Nfft))./realNoise),SNR_values_db,20*log10(1/sqrt(2.0*enb.CellRefP*double(info.Nfft))./noiseEstimation),SNR_values_db,20*log10(1/sqrt(2.0*enb.CellRefP*double(info.Nfft))./noiseEstimation2))
% legend('real','seu','meu')
plot(SNR_values_db, mean(preEVM,2), ...
    SNR_values_db, mean(postEVM_mmse,2), ...
    SNR_values_db, mean(postEVM_liblte,2))
legend('No Eq','MMSE-cubic','MMSE-lin','MMSE-liblte')
grid on

