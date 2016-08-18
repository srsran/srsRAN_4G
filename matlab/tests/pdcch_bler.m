
%% PDCCH Blind Search and DCI Decoding + PCFICH encoding/decoding

%% Cell-Wide Settings
% A structure |enbConfig| is used to configure the eNodeB.
clear

Npackets = 60;
SNR_values = linspace(2,6,6);

txCFI = 3;
enbConfig.NDLRB = 15;                % No of Downlink RBs in total BW
enbConfig.CyclicPrefix = 'Normal';  % CP length
enbConfig.CFI = txCFI;                                                                                                                                                                                                                                                                                                                                                                                                                                             % 4 PDCCH symbols as NDLRB <= 10
enbConfig.Ng = 'One';             % HICH groups
enbConfig.CellRefP = 1;             % 1-antenna ports
enbConfig.NCellID = 0;             % Physical layer cell identity
enbConfig.NSubframe = 5;            % Subframe number 0
enbConfig.DuplexMode = 'FDD';       % Frame structure
enbConfig.PHICHDuration = 'Normal';
C_RNTI = 1;                         % 16-bit UE-specific mask

%% Setup Fading channel model 
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

% Setup channel equalizer
cec.PilotAverage = 'UserDefined';     % Type of pilot averaging
cec.FreqWindow = 9;                   % Frequency window size
cec.TimeWindow = 9;                   % Time window size
cec.InterpType = 'linear';             % 2D interpolation type
cec.InterpWindow = 'Causal';        % Interpolation window type
cec.InterpWinSize = 1;                % Interpolation window size

%% DCI Message Generation
% Generate a DCI message to be mapped to the PDCCH.

dciConfig.DCIFormat = 'Format1A';   % DCI message format
dciConfig.Allocation.RIV = 26;      % Resource indication value

% Create DCI message for given configuration
[dciMessage, dciMessageBits] = lteDCI(enbConfig, dciConfig);

%% DCI Channel Coding

% Do not include RNTI if Common Search space
if C_RNTI<65535
    pdcchConfig.RNTI = C_RNTI;            % Radio network temporary identifier
end
pdcchConfig.PDCCHFormat = 3;          % PDCCH format
ueConfig.RNTI = C_RNTI;

candidates = ltePDCCHSpace(enbConfig, pdcchConfig, {'bits', '1based'});

% Include now RNTI in pdcch
pdcchConfig.RNTI = C_RNTI;        

% DCI message bits coding to form coded DCI bits
codedDciBits = lteDCIEncode(pdcchConfig, dciMessageBits);

%% PDCCH Bits Generation

pdcchDims = ltePDCCHInfo(enbConfig);

% Initialize elements with -1 to indicate that all the bits are unused
pdcchBitsTx = -1*ones(pdcchDims.MTot, 1);


Ncad=1;

% Map PDCCH payload on available UE-specific candidate. In this example the
% first available candidate is used to map the coded DCI bits.
pdcchBitsTx ( candidates(Ncad, 1) : candidates(Ncad, 2) ) = codedDciBits;

%% PDCCH Complex-Valued Modulated Symbol Generation

pdcchSymbolsTx = ltePDCCH(enbConfig, pdcchBitsTx);
pdcchIndices = ltePDCCHIndices(enbConfig,{'1based'});
subframe_tx = lteDLResourceGrid(enbConfig);
subframe_tx(pdcchIndices) = pdcchSymbolsTx;

%% PCFICH
cfiCodeword = lteCFI(enbConfig);
pcfichSymbols = ltePCFICH(enbConfig,cfiCodeword);
pcfichIndices = ltePCFICHIndices(enbConfig,'1based');
subframe_tx(pcfichIndices) = pcfichSymbols;

%% Add references
cellRsSym = lteCellRS(enbConfig);
cellRsInd = lteCellRSIndices(enbConfig);
subframe_tx(cellRsInd) = cellRsSym;

[txWaveform, info] = lteOFDMModulate(enbConfig,subframe_tx);
cfg.SamplingRate = info.SamplingRate;

addpath('../../build/srslte/lib/phch/test')

decoded = zeros(size(SNR_values));
decoded_cfi = zeros(size(SNR_values));
decoded_srslte = zeros(size(SNR_values));
decoded_cfi_srslte = zeros(size(SNR_values));

for snr_idx=1:length(SNR_values)
    SNRdB = SNR_values(snr_idx);
    SNR = 10^(SNRdB/10);    % Linear SNR  
    N0  = 1/(sqrt(2.0*enbConfig.CellRefP*double(info.Nfft))*SNR);
    for i=1:Npackets

        enbConfigRx=enbConfig; 

        rxWaveform = sum(txWaveform,2);
        
        %% Fading
        rxWaveform = lteFadingChannel(cfg,rxWaveform);
        
        %% Noise Addition
        noise = N0*complex(randn(size(rxWaveform)), randn(size(rxWaveform)));  % Generate noise
        rxWaveform = rxWaveform + noise; 
       
        %% Demodulate 
        subframe_rx = lteOFDMDemodulate(enbConfigRx, rxWaveform);

        % Perform channel estimation
        [hest, nest] = lteDLChannelEstimate(enbConfigRx, cec, subframe_rx);

        [pcfichSymbolsRx, pcfichSymbolsHest] = lteExtractResources(pcfichIndices(:,1), subframe_rx, hest);

        %% PCFICH decoding
        [pcfichBits, pcfichSymbols] = ltePCFICHDecode(enbConfigRx,pcfichSymbolsRx, pcfichSymbolsHest, nest);
        rxCFI = lteCFIDecode(pcfichBits);
        
        decoded_cfi(snr_idx) = decoded_cfi(snr_idx) + (rxCFI == txCFI); 
        
        %% PDCCH Decoding
        enbConfigRx.CFI = rxCFI; 
        pdcchIndicesRx = ltePDCCHIndices(enbConfigRx,{'1based'});
        [pdcchRx, pdcchHest] = lteExtractResources(pdcchIndicesRx(:,1), subframe_rx, hest);
        [pdcchBits, pdcchSymbols] = ltePDCCHDecode(enbConfigRx, pdcchRx, pdcchHest, nest);

        %% Blind Decoding using DCI Search        
        [rxDCI, rxDCIBits] = ltePDCCHSearch(enbConfigRx, ueConfig, pdcchBits);
        decoded(snr_idx) = decoded(snr_idx) + (length(rxDCI)>0);
        
        %% Same with srsLTE
        [rxCFI_srslte, pcfichRx2, pcfichSymbols2] = srslte_pcfich(enbConfigRx, subframe_rx);
        decoded_cfi_srslte(snr_idx) = decoded_cfi_srslte(snr_idx) + (rxCFI_srslte == txCFI); 
        enbConfigRx.CFI = txCFI;
        [found_srslte, pdcchBits2, pdcchRx2, pdcchSymbols2, hest2] = srslte_pdcch(enbConfigRx, ueConfig.RNTI, subframe_rx, hest, nest);
        decoded_srslte(snr_idx) = decoded_srslte(snr_idx)+found_srslte;
    end
    fprintf('SNR: %.1f\n',SNRdB)
end

if (Npackets>1)
    semilogy(SNR_values,1-decoded/Npackets,'bo-',...
             SNR_values,1-decoded_cfi/Npackets,'bx:',...
             SNR_values,1-decoded_srslte/Npackets, 'ro-',...
             SNR_values,1-decoded_cfi_srslte/Npackets,'rx:')
    grid on
    legend('Matlab all','Matlab cfi', 'srsLTE all', 'srsLTE cfi')
    xlabel('SNR (dB)')
    ylabel('BLER')
    axis([min(SNR_values) max(SNR_values) 1/Npackets/10 1])
else
    
    n=min(length(pdcchSymbols),length(pdcchSymbols2));
    subplot(2,1,1)
    plot(abs(pdcchSymbols(1:n)-pdcchSymbols2(1:n)))
    n=min(length(pdcchBits),length(pdcchBits2));
    subplot(2,1,2)
    pdcchBitsTx(pdcchBitsTx==-1)=0;
    plot(abs((pdcchBitsTx(1:n)>0.1)-(pdcchBits2(1:n)>0.1)))

    subplot(1,1,1)
    plot(1:180,real(hest(:,1,1,1)),1:180,real(hest2(1:180)))
    
    disp(decoded)
    disp(decoded_srslte)
end

