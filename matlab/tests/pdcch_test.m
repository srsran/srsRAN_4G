%% PDCCH Blind Search and DCI Decoding

%% Cell-Wide Settings
% A structure |enbConfig| is used to configure the eNodeB.

Npackets = 50;
SNR_values =linspace(-5,3,8);

enbConfig.NDLRB = 15;                % No of Downlink RBs in total BW
enbConfig.CyclicPrefix = 'Normal';  % CP length
enbConfig.CFI = 3;                                                                                                                                                                                                                                                                                                                                                                                                                           ;                  % 4 PDCCH symbols as NDLRB <= 10
enbConfig.Ng = 'Sixth';             % HICH groups
enbConfig.CellRefP = 2;             % 1-antenna ports
enbConfig.NCellID = 10;             % Physical layer cell identity
enbConfig.NSubframe = 0;            % Subframe number 0
enbConfig.DuplexMode = 'FDD';       % Frame structure
enbConfig.PHICHDuration = 'Normal';

%% DCI Message Generation
% Generate a DCI message to be mapped to the PDCCH.

dciConfig.DCIFormat = 'Format1A';   % DCI message format
dciConfig.Allocation.RIV = 26;      % Resource indication value

% Create DCI message for given configuration
[dciMessage, dciMessageBits] = lteDCI(enbConfig, dciConfig);

%% DCI Channel Coding

C_RNTI = 65535;                         % 16-bit UE-specific mask
pdcchConfig.RNTI = C_RNTI;            % Radio network temporary identifier
pdcchConfig.PDCCHFormat = 3;          % PDCCH format

% DCI message bits coding to form coded DCI bits
codedDciBits = lteDCIEncode(pdcchConfig, dciMessageBits);

%% PDCCH Bits Generation

pdcchDims = ltePDCCHInfo(enbConfig);

% Initialize elements with -1 to indicate that all the bits are unused
pdcchBits = -1*ones(pdcchDims.MTot, 1);

% Perform search space for UE-specific control channel candidates.
candidates = ltePDCCHSpace(enbConfig, pdcchConfig, {'bits', '1based'});

Ncad=randi(length(candidates),1,1);

% Map PDCCH payload on available UE-specific candidate. In this example the
% first available candidate is used to map the coded DCI bits.
pdcchBits ( candidates(Ncad, 1) : candidates(Ncad, 2) ) = codedDciBits;

%% PDCCH Complex-Valued Modulated Symbol Generation

pdcchSymbols = ltePDCCH(enbConfig, pdcchBits);

pdcchIndices = ltePDCCHIndices(enbConfig,{'1based'});

decoded = zeros(size(SNR_values));
decoded_liblte = zeros(size(SNR_values));

Nports = enbConfig.CellRefP; 
ueConfig.RNTI = C_RNTI;

subframe_tx = lteDLResourceGrid(enbConfig);
subframe_tx(pdcchIndices) = pdcchSymbols;

addpath('../../debug/lte/phy/lib/phch/test')

parfor snr_idx=1:length(SNR_values)
    SNRdB = SNR_values(snr_idx);
    for i=1:Npackets

        %% Noise Addition
        SNR = 10^(SNRdB/10);    % Linear SNR  

        N0  = 1/(sqrt(2.0*Nports)*SNR);
        noise = N0*complex(randn(size(subframe_tx)), randn(size(subframe_tx)));  % Generate noise

        subframe_rx = sum(subframe_tx + noise,3);    % Add noise to PDCCH symbols

        pdcchSymbolsNoisy = subframe_rx(pdcchIndices(:,1));

        %% PDCCH Decoding
        recPdcchBits = ltePDCCHDecode(enbConfig, pdcchSymbolsNoisy);

        %% Blind Decoding using DCI Search        
        [rxDCI, rxDCIBits] = ltePDCCHSearch(enbConfig, ueConfig, recPdcchBits);
        decoded(snr_idx) = decoded(snr_idx) + length(rxDCI);
        
        [found_liblte, llr, viterbi_in] = liblte_pdcch(enbConfig, ueConfig.RNTI, subframe_rx);

        decoded_liblte(snr_idx) = decoded_liblte(snr_idx)+found_liblte;
    end
    fprintf('SNR: %.1f\n',SNRdB)
end

if (Npackets>1)
    plot(SNR_values,1-decoded/Npackets,SNR_values,1-decoded_liblte/Npackets)
    grid on
    legend('Matlab','libLTE')
else
    disp(decoded_liblte)
end

