%% LTE Downlink Channel Estimation and Equalization

%% Cell-Wide Settings

clear

plot_noise_estimation_only=false;

SNR_values_db=linspace(0,30,8);
Nrealizations=10;

w1=0.1;
w2=0.2;

enb.NDLRB = 6;                 % Number of resource blocks

enb.CellRefP = 1;               % One transmit antenna port
enb.NCellID = 0;                % Cell ID
enb.CyclicPrefix = 'Normal';    % Normal cyclic prefix
enb.DuplexMode = 'FDD';         % FDD

K=enb.NDLRB*12;
P=K/6;

%% Channel Model Configuration
cfg.Seed = 0;                  % Random channel seed
cfg.InitTime = 0;
cfg.NRxAnts = 1;               % 1 receive antenna
cfg.DelayProfile = 'EVA';     

% doppler 5, 70 300

cfg.DopplerFreq = 5;          % 120Hz Doppler frequency
cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
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
cec.InterpWindow = 'Causal';       % Interpolation window type
cec.InterpWinSize = 1;               % Interpolation window size

%% Subframe Resource Grid Size

gridsize = lteDLResourceGridSize(enb);
Ks = gridsize(1);    % Number of subcarriers
L = gridsize(2);    % Number of OFDM symbols in one subframe
Ports = gridsize(3);    % Number of transmit antenna ports

%% Allocate memory
Ntests=4;
hest=cell(1,Ntests);
tmpnoise=cell(1,Ntests);
for i=1:Ntests
    hest{i}=zeros(K,140);
    tmpnoise{i}=zeros(1,10);
end
hls=zeros(4,4*P*10);
MSE=zeros(Ntests,Nrealizations,length(SNR_values_db));
noiseEst=zeros(Ntests,Nrealizations,length(SNR_values_db));

legends={'matlab','ls',num2str(w1),num2str(w2)};
colors={'bo-','rx-','m*-','k+-','c+-'};
colors2={'b-','r-','m-','k-','c-'};

addpath('../../build/srslte/lib/ch_estimation/test')

offset=-1;

for nreal=1:Nrealizations
%% Transmit Resource Grid
txGrid = [];

%% Payload Data Generation
% Number of bits needed is size of resource grid (K*L*P) * number of bits
% per symbol (2 for QPSK)
numberOfBits = Ks*L*Ports*2; 

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

txGrid([1:5 68:72],6:7) = ones(10,2);

%% OFDM Modulation

[txWaveform,info] = lteOFDMModulate(enb,txGrid); 
txGrid = txGrid(:,1:140);

%% SNR Configuration
for snr_idx=1:length(SNR_values_db)
SNRdB = SNR_values_db(snr_idx);             % Desired SNR in dB
SNR = 10^(SNRdB/20);    % Linear SNR  

fprintf('SNR=%.1f dB\n',SNRdB)

%% Fading Channel

cfg.SamplingRate = info.SamplingRate;
[rxWaveform, chinfo] = lteFadingChannel(cfg,txWaveform);

%% Additive Noise

% Calculate noise gain
N0 = 1/(sqrt(2.0*enb.CellRefP*double(info.Nfft))*SNR);

% Create additive white Gaussian noise
noise = N0*complex(randn(size(rxWaveform)),randn(size(rxWaveform)));   

% Add noise to the received time domain waveform
rxWaveform_nonoise = rxWaveform;
rxWaveform = rxWaveform + noise;

%% Synchronization

if (offset==-1) 
    offset = lteDLFrameOffset(enb,rxWaveform);
end

rxWaveform = rxWaveform(1+offset:end,:);
rxWaveform_nonoise = rxWaveform_nonoise(1+offset:end,:);

%% OFDM Demodulation
rxGrid = lteOFDMDemodulate(enb,rxWaveform);
rxGrid = rxGrid(:,1:140);

rxGrid_nonoise = lteOFDMDemodulate(enb,rxWaveform_nonoise);
rxGrid_nonoise = rxGrid_nonoise(:,1:140);

% True channel
h=rxGrid_nonoise./(txGrid);

for i=1:10
    enb.NSubframe=i-1;

    rxGrid_sf = rxGrid(:,(i-1)*14+1:i*14); 
    
    %% Channel Estimation with Matlab
    [hest{1}(:,(1:14)+(i-1)*14), tmpnoise{1}(i)] = ...
                    lteDLChannelEstimate(enb,cec,rxGrid_sf);
    tmpnoise{1}(i)=tmpnoise{1}(i)*sqrt(2)*enb.CellRefP;
    
    %% LS-Linear estimation with srsLTE
    [hest{2}(:,(1:14)+(i-1)*14), tmpnoise{2}(i)] = srslte_chest_dl(enb,rxGrid_sf);
    
    %% LS-Linear + averaging with srsLTE
    [hest{3}(:,(1:14)+(i-1)*14), tmpnoise{3}(i)] = srslte_chest_dl(enb,rxGrid_sf,w1);
    
    %% LS-Linear + more averaging with srsLTE
    [hest{4}(:,(1:14)+(i-1)*14), tmpnoise{4}(i)] = srslte_chest_dl(enb,rxGrid_sf,w2);

end

%% Average noise estimates over all frame
for i=1:Ntests
    noiseEst(i,nreal,snr_idx)=mean(tmpnoise{i});
end

%% Compute MSE 
for i=1:Ntests
    MSE(i,nreal,snr_idx)=mean(mean(abs(h-hest{i}).^2));
    fprintf('MSE test %d: %f\n',i, 10*log10(MSE(i,nreal,snr_idx)));
end

%% Plot a single realization
if (length(SNR_values_db) == 1)
    sym=1;
    ref_idx=1:P;
    ref_idx_x=[1:6:K];% (292:6:360)-216];% 577:6:648];
    n=1:(K*length(sym));
    for i=1:Ntests
        plot(n,abs(reshape(hest{i}(:,sym),1,[])),colors2{i});
        hold on;
    end
    plot(n, abs(h(:,sym)),'g-')
%    plot(ref_idx_x,real(hls(3,ref_idx)),'ro');
    hold off;
    tmp=cell(Ntests+1,1);
    for i=1:Ntests
        tmp{i}=legends{i};
    end
    tmp{Ntests+1}='Real';
    legend(tmp)

    xlabel('SNR (dB)')
    ylabel('Channel Gain')
    grid on;
        
    fprintf('Mean MMSE Robust %.2f dB\n', 10*log10(MSE(4,nreal,snr_idx)))
    fprintf('Mean MMSE matlab %.2f dB\n', 10*log10(MSE(1,nreal,snr_idx)))
end

end
end


%% Compute average MSE and noise estimation
mean_mse=mean(MSE,2);
mean_snr=10*log10(1./mean(noiseEst,2));


%% Plot average over all SNR values
if (length(SNR_values_db) > 1)
    subplot(1,2,1)
    for i=1:Ntests
        plot(SNR_values_db, 10*log10(mean_mse(i,:)),colors{i})
        hold on;
    end
    hold off;
    legend(legends);
    grid on
    xlabel('SNR (dB)')
    ylabel('MSE (dB)')
    
    subplot(1,2,2)
    plot(SNR_values_db, SNR_values_db,'k:')
    hold on;
    for i=1:Ntests
        plot(SNR_values_db, mean_snr(i,:), colors{i})
    end
    hold off
    tmp=cell(Ntests+1,1);
    tmp{1}='Theory';
    for i=2:Ntests+1
        tmp{i}=legends{i-1};
    end
    legend(tmp)
    grid on
    xlabel('SNR (dB)')
    ylabel('Estimated SNR (dB)')
end

