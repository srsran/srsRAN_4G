enb=struct('NCellID',313,'NDLRB',75,'NSubframe',5,'CFI',1,'CyclicPrefix','Normal','CellRefP',2,'Ng','One','PHICHDuration','Normal','DuplexMode','FDD');

RNTI=65535;

addpath('../../build/srslte/lib/phch/test')

cec.PilotAverage = 'UserDefined';     % Type of pilot averaging
cec.FreqWindow = 1;                   % Frequency window size    
cec.TimeWindow = 1;                   % Time window size    
cec.InterpType = 'linear';             % 2D interpolation type
cec.InterpWindow = 'Causal';        % Interpolation window type
cec.InterpWinSize = 1;                % Interpolation window size  

%subframe_rx=lteOFDMDemodulate(enb,x);
subframe_rx=reshape(x,[],14);
[hest,nest] = lteDLChannelEstimate(enb, cec, subframe_rx);    
    
% Search PDCCH
pdcchIndices = ltePDCCHIndices(enb); 
[pdcchRx, pdcchHest] = lteExtractResources(pdcchIndices, subframe_rx, hest);
[dciBits, pdcchSymbols] = ltePDCCHDecode(enb, pdcchRx, pdcchHest, nest);
pdcch = struct('RNTI', RNTI);  
[dci, dcidecBits] = ltePDCCHSearch(enb, pdcch, dciBits); % Search PDCCH for DCI                

if ~isempty(dci)
        
    dci = dci{1};
    disp(dci);
    
    % Get the PDSCH configuration from the DCI
    [pdsch, trblklen] = hPDSCHConfiguration(enb, dci, pdcch.RNTI);
    pdsch.NTurboDecIts = 16;
    %pdsch.Modulation =  {'64QAM'};
    pdsch.RV=0;
    fprintf('PDSCH settings after DCI decoding:\n');
    disp(pdsch);

    fprintf('Decoding PDSCH...\n\n');        
    % Get PDSCH indices
    [pdschIndices,pdschIndicesInfo] = ltePDSCHIndices(enb, pdsch, pdsch.PRBSet);
    [pdschRx, pdschHest] = lteExtractResources(pdschIndices, subframe_rx, hest);
    % Decode PDSCH 
    [dlschBits,pdschSymbols] = ltePDSCHDecode(enb, pdsch, pdschRx, pdschHest, 0);
    [sib1, crc] = lteDLSCHDecode(enb, pdsch, trblklen, dlschBits);

    
    [dec2, data, pdschRx2, pdschSymbols2, e_bits, ce] = srslte_pdsch(enb, pdsch, ... 
                                                        trblklen, ...
                                                        subframe_rx);

    
    subplot(2,1,1)
    scatter(real(pdschSymbols{1}),imag(pdschSymbols{1}))
    subplot(2,1,2)
    scatter(real(pdschSymbols2),imag(pdschSymbols2))

    if crc == 0
        fprintf('PDSCH Matlab OK.\n\n');
    else
        fprintf('PDSCH Matlab ERROR.\n\n');
    end

    if dec2 == 1
        fprintf('PDSCH srsLTE OK.\n\n');
    else
        fprintf('PDSCH srsLTE ERROR.\n\n');
    end

    else
        % indicate that DCI decoding failed 
        fprintf('DCI decoding failed.\n\n');
end

%indices=indices+1;
%plot(t,indices(t),t,pdschIndices(t))
    