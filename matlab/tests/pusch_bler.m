%% Plot PUSCH BLER vs SNR for PUSCH without equalization
clear
transportBlkSize=904;
modulation='64QAM';
rvValues=0;
SNR=linspace(-5,0.0,8);
Nblocks=30;

addpath('../../build/srslte/lib/phch/test')

% Subframe configuration
ueConfig.NCellID = 100;
ueConfig.CyclicPrefixUL = 'Normal';
ueConfig.NSubframe = 0;
ueConfig.NULRB = 6;
ueConfig.Shortened = 0;
ueConfig.NTxAnts = 1; 
ueConfig.RNTI = 1; 
ueConfig.DuplexMode='FDD';
ueConfig.Hopping = 'Off';
ueConfig.SeqGroup = 0;
ueConfig.CyclicShift = 0;
ueConfig.Shortened = 0;

% Transmission mode configuration for PUSCH
puschConfig.NLayers = 1;
puschConfig.TxScheme = 'Port0';
puschConfig.Modulation = modulation;
puschConfig.NTurboDecIts = 5;
puschConfig.PRBSet = (0:ueConfig.NULRB-1)';
puschConfig.NBundled = 0; 

% Configure control channels
puschConfig.OCQI = 0; 
puschConfig.ORI  = 0;
puschConfig.OACK = 0;

puschConfig.BetaCQI = 2; 
puschConfig.BetaRI = 3;
puschConfig.BetaACK = 1;

info=lteULSCHInfo(ueConfig,puschConfig,transportBlkSize,puschConfig.OCQI,puschConfig.ORI,puschConfig.OACK);
puschConfig.QdACK=info.QdACK;
puschConfig.QdRI=info.QdRI;
puschConfig.QdCQI=info.QdCQI;

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
nErrorsCtrl_mat = zeros(length(SNR),3);
nErrors_srs = zeros(length(SNR),length(rvValues));
nErrorsCtrl_srs = zeros(length(SNR),3);

for k = 1:length(SNR);
    subframe=cell(length(rvValues));
    puschIdx=ltePUSCHIndices(ueConfig,puschConfig);
    for i=1:length(rvValues)
        subframe{i} = lteULResourceGrid(ueConfig);
    end
    blkCounter = 0;
    for l = 1:Nblocks;
        % UL-SCH data bits
        ulschBits = randi([0 1],transportBlkSize,1);
        softBuffer = {};     
        
        % Control bits
        cqi = randi([0 1], puschConfig.OCQI,1);
        ri  = randi([0 1], puschConfig.ORI,1);
        ack = randi([0 1], puschConfig.OACK,1);
        
        for rvIndex = 1:length(rvValues)
            % ULSCH transport channel
            puschConfig.RV = rvValues(rvIndex);
            puschPayload = lteULSCH(ueConfig, puschConfig, ulschBits, cqi, ri, ack);

            % PUSCH modulated symbols
            puschSymbols = ltePUSCH(ueConfig, puschConfig, puschPayload);
            puschSize = size(puschSymbols);

            % Addition of noise
            noise = (1/noiseVarfactor)*sqrt(1/snr(k))*complex(randn(puschSize),randn(puschSize));
            noisySymbols = puschSymbols + noise;

            subframe{rvIndex}(puschIdx)=noisySymbols;
            
            % PUSCH Rx-side
            [rxCW, puschSymbolsRx] = ltePUSCHDecode(ueConfig, puschConfig, noisySymbols);
            if (iscell(rxCW))
                rxCW_=rxCW{1};
            else
                rxCW_=rxCW;
            end
            
            % UL-SCH turbo decoding
            [rxBits, blkCRCerr, softBuffer,ccqi,cri,cack] = lteULSCHDecode2(ueConfig, puschConfig, ... 
                transportBlkSize, rxCW_, softBuffer);

            % Add errors to previous error counts
            nErrors_mat(k,rvIndex) = nErrors_mat(k,rvIndex)+blkCRCerr;     

            if (rvIndex==1) 
                ack_rx=lteACKDecode(puschConfig,cack);
                ri_rx=lteRIDecode(puschConfig,cri);
                cqi_rx=lteCQIDecode(puschConfig,ccqi);
                
                nErrorsCtrl_mat(k,1) = nErrorsCtrl_mat(k,1)+(sum(ack_rx~=ack)>0);                                 
                nErrorsCtrl_mat(k,2) = nErrorsCtrl_mat(k,2)+(sum(ri_rx~=ri)>0);                                                 
                nErrorsCtrl_mat(k,3) = nErrorsCtrl_mat(k,3)+(sum(cqi_rx~=cqi)>0);                                 
            end
        end
        
        % Same with srsLTE 
        [okSRSLTE, cqi_rx_srs, ri_rx_srs, ack_rx_srs] = srslte_puscht(ueConfig, puschConfig, ...
            transportBlkSize, subframe, ones(size(subframe{1})), 0);
        
        nErrors_srs(k,rvIndex) = nErrors_srs(k,rvIndex)+~okSRSLTE;          
        
        if (rvIndex==1) 
            nErrorsCtrl_srs(k,1) = nErrorsCtrl_srs(k,1)+(sum(ack_rx_srs~=ack)>0);                                 
            nErrorsCtrl_srs(k,2) = nErrorsCtrl_srs(k,2)+(sum(ri_rx_srs~=ri)>0);                                                 
            nErrorsCtrl_srs(k,3) = nErrorsCtrl_srs(k,3)+(sum(cqi_rx_srs~=cqi)>0);                                 
        end
    end
    fprintf('SNR=%.1f dB, BLER_mat=%.2f, BLER_srs=%.2f, BLER_ack=%.2f/%.2f, BLER_ri=%.2f/%.2f, BLER_cqi=%.2f/%.2f\n',... 
        SNR(k),nErrors_mat(k,rvIndex)/Nblocks, nErrors_srs(k,rvIndex)/Nblocks, ... 
               nErrorsCtrl_mat(k,1)/Nblocks, nErrorsCtrl_srs(k,1)/Nblocks, ...
               nErrorsCtrl_mat(k,3)/Nblocks, nErrorsCtrl_srs(k,2)/Nblocks, ...
               nErrorsCtrl_mat(k,2)/Nblocks, nErrorsCtrl_srs(k,3)/Nblocks);    
end

puschBLER_mat = nErrors_mat./Nblocks;
puschBLER_mat(puschBLER_mat==0)=10^-10;

puschBLER_srs = nErrors_srs./Nblocks;
puschBLER_srs(puschBLER_srs==0)=10^-10;


puschCtrlBLER_mat = nErrorsCtrl_mat./Nblocks;
puschCtrlBLER_mat(puschCtrlBLER_mat==0)=10^-10;

puschCtrlBLER_srs = nErrorsCtrl_srs./Nblocks;
puschCtrlBLER_srs(puschCtrlBLER_srs==0)=10^-10;

if (Nblocks == 1 && length(SNR) == 1)
else
    
    ctrlplot=1;
    if (puschConfig.OCQI+puschConfig.ORI+puschConfig.OACK>0)
        ctrlplot=2;
    end
    
    subplot(ctrlplot,1,1)
    semilogy(SNR,puschBLER_mat,SNR,puschBLER_srs);    
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
    axis([min(SNR) max(SNR) 10^-4 1])

    if (ctrlplot==2)
        subplot(2,1,2)
        semilogy(SNR,puschCtrlBLER_mat,SNR,puschCtrlBLER_srs)
        grid on
        xlabel('Eb/No (dB)')
        ylabel('BLER')
        leg=[];
        leg=strvcat(leg,'Matlab ACK','Matlab RI', 'Matlab CQI');
        leg=strvcat(leg,'srsLTE ACK','srsLTE RI', 'srsLTE CQI');
        legend(leg);
        axis([min(SNR) max(SNR) 10^-4 1])
    end
end
