%% Plot PDSCH BLER vs SNR for PDSCH without equalization
clear
transportBlkSize=75376;
modulation='64QAM';
rvValues=[0 2 3 1];
SNR=linspace(-2.9,-2.0,8);
Nblocks=30;

addpath('../../build/srslte/lib/phch/test')

% Subframe configuration
enbConfig.NCellID = 100;
enbConfig.CyclicPrefix = 'Normal';
enbConfig.NSubframe = 1;
enbConfig.CellRefP = 1;
enbConfig.NDLRB = 100;
enbConfig.CFI = 1; 
enbConfig.DuplexMode='FDD';

% Transmission mode configuration for PDSCH
pdschConfig.NLayers = 1;
pdschConfig.TxScheme = 'Port0';
pdschConfig.Modulation = {modulation};
pdschConfig.RNTI = 100;
pdschConfig.NTurboDecIts = 5;
pdschConfig.PRBSet = (0:enbConfig.NDLRB-1)';

switch (modulation)
    case 'QPSK'
        bitsPerSym = 2;
    case '16QAM'
        bitsPerSym = 4;
    case '64QAM'
        bitsPerSym = 6;
end
noiseVarfactor = sqrt(2*bitsPerSym);
snr = 10.^(SNR/10);

nErrors_mat = zeros(length(SNR),length(rvValues));
nErrors_srs = zeros(length(SNR),length(rvValues));
        
for k = 1:length(SNR);
    subframe=cell(length(rvValues));
    pdschIdx=ltePDSCHIndices(enbConfig,pdschConfig,pdschConfig.PRBSet);
    for i=1:length(rvValues)
        subframe{i} = lteDLResourceGrid(enbConfig);
    end
    blkCounter = 0;
    for l = 1:Nblocks;
        % DL-SCH data bits
        dlschBits = randi([0 1],transportBlkSize,1);
        softBuffer = {};     
        for rvIndex = 1:length(rvValues)
            % DLSCH transport channel
            pdschConfig.RV = rvValues(rvIndex);
            pdschPayload = lteDLSCH(enbConfig, pdschConfig, length(pdschIdx)*bitsPerSym, dlschBits);

            % PDSCH modulated symbols
            pdschSymbols = ltePDSCH(enbConfig, pdschConfig, pdschPayload);
            pdschSize = size(pdschSymbols);

            % Addition of noise
            noise = (1/noiseVarfactor)*sqrt(1/snr(k))*complex(randn(pdschSize),randn(pdschSize));
            noisySymbols = pdschSymbols + noise;

            subframe{rvIndex}(pdschIdx)=noisySymbols;
            
            % PDSCH Rx-side
            rxCW = ltePDSCHDecode(enbConfig, pdschConfig, noisySymbols);
            
            % DL-SCH turbo decoding
            [rxBits, blkCRCerr, softBuffer] = lteDLSCHDecode(enbConfig, pdschConfig, transportBlkSize, rxCW{1}, softBuffer);
            
            % Add errors to previous error counts
            nErrors_mat(k,rvIndex) = nErrors_mat(k,rvIndex)+blkCRCerr;             
        end
        
        % Same with srsLTE 
        [okSRSLTE, data, pdschRx, pdschSymbols, cws] = srslte_pdsch(enbConfig, pdschConfig, ...
            transportBlkSize, subframe, ones(size(subframe{1})), 0);
        
        nErrors_srs(k,rvIndex) = nErrors_srs(k,rvIndex)+~okSRSLTE;          
    end
    fprintf('SNR=%.1f dB, BLER_mat=%f, BLER_srs=%f\n',SNR(k),nErrors_mat(k,rvIndex)/Nblocks, nErrors_srs(k,rvIndex)/Nblocks);    
end

PDSCHBLER_MAT = nErrors_mat./Nblocks;
PDSCHBLER_MAT(PDSCHBLER_MAT==0)=10^-10;

PDSCHBLER_SRS = nErrors_srs./Nblocks;
PDSCHBLER_SRS(PDSCHBLER_SRS==0)=10^-10;

if (Nblocks == 1 && length(SNR) == 1)
else
    semilogy(SNR,PDSCHBLER_MAT,SNR,PDSCHBLER_SRS)
    grid on
    xlabel('Eb/No (dB)')
    ylabel('BLER')
    leg=[];
    for rvIndex = 1:length(rvValues)
        leg=strvcat(leg,sprintf('Matlab rv=%d',rvValues(rvIndex)));
    end
    for rvIndex = 1:length(rvValues)
        leg=strvcat(leg,sprintf('srsLTE rv=%d',rvValues(rvIndex)));
    end
    legend(leg);
    axis([min(SNR) max(SNR) 10^-4 1])
end
