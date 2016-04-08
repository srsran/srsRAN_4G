clear

Nblock=[3];
SNR_values_db=100;%linspace(-4,0,6);
Nrealizations=1;
enb = struct('NCellID',62,'NDLRB',50,'CellRefP',2,'CyclicPrefix','Normal','DuplexMode','FDD',... 
    'NSubframe',0,'PHICHDuration','Normal','Ng','One','NFrame',101,'TotSubframes',40);
        
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

cec.PilotAverage = 'UserDefined';     % Type of pilot averaging
cec.FreqWindow = 9;                   % Frequency window size
cec.TimeWindow = 9;                   % Time window size
cec.InterpType = 'linear';             % 2D interpolation type
cec.InterpWindow = 'Centered';        % Interpolation window type
cec.InterpWinSize = 1;                % Interpolation window size

griddims = lteResourceGridSize(enb); % Resource grid dimensions
L = griddims(2);    

% Generate signal
mib = lteMIB(enb);
bchCoded = lteBCH(enb,mib);
mibCRC = lteCRCEncode(mib,'16');
mibCoded = lteConvolutionalEncode(mibCRC);
pbchSymbolsTx = ltePBCH(enb,bchCoded);
pbchIndtx = ltePBCHIndices(enb);
subframe_tx = lteDLResourceGrid(enb);
rs = lteCellRS(enb);
rsind = lteCellRSIndices(enb);
subframe_tx(rsind)=rs;

NofPortsTx=enb.CellRefP;

addpath('../../build/srslte/lib/phch/test')

txWaveform=cell(length(Nblock));
rxWaveform=cell(length(Nblock));
for n=1:length(Nblock)
    subframe_tx2=subframe_tx;
    subframe_tx2(pbchIndtx)=pbchSymbolsTx(Nblock(n)*240+1:(Nblock(n)+1)*240,:); 
    [txWaveform{n},info] = lteOFDMModulate(enb, subframe_tx2, 0);
    cfg.SamplingRate = info.SamplingRate;
end

error=zeros(length(SNR_values_db),2);
for snr_idx=1:length(SNR_values_db)
    SNRdB = SNR_values_db(snr_idx);             % Desired SNR in dB
    SNR = 10^(SNRdB/10);    % Linear SNR  

    errorReal = zeros(Nrealizations,2);
    for i=1:Nrealizations

        for n=1:length(Nblock)

            %rxWaveform = lteFadingChannel(cfg,sum(txWaveform,2));
            rxWaveform{n} = sum(txWaveform{n},2);

            %% Additive Noise
            N0 = 1/(sqrt(2.0*double(enb.CellRefP)*double(info.Nfft))*SNR);

            % Create additive white Gaussian noise
            noise = N0*complex(randn(size(rxWaveform{n})),randn(size(rxWaveform{n})));   

            rxWaveform{n} = noise + rxWaveform{n};

            % Number of OFDM symbols in a subframe
            % OFDM demodulate signal
            rxgrid = lteOFDMDemodulate(enb, rxWaveform{n}, 0);

            % Perform channel estimation
            %enb.CellRefP=2;
            [hest, nest] = lteDLChannelEstimate(enb, cec, rxgrid(:,1:L,:));

            pbchIndices = ltePBCHIndices(enb);
            [pbchRx, pbchHest] = lteExtractResources(pbchIndices, rxgrid(:,1:L,:), ... 
                hest(:,1:L,:,:));

            % Decode PBCH
            [bchBits, pbchSymbols, nfmod4, mib, nof_ports] = ltePBCHDecode(enb, pbchRx, pbchHest, nest);

            if (nof_ports ~= NofPortsTx)
                errorReal(i,1)=1;
            end
        end
        
        %enb.CellRefP=NofPortsTx;
        [nof_ports2, pbchSymbols2, pbchBits, ce, ce2, pbchRx2, pbchHest2, mod2, codedbits]= ...
            srslte_pbch(enb, rxWaveform);
           
        subplot(2,1,1)
        plot(abs((bchCoded(1:960)>0)-(pbchBits(1:960)>0)))
        subplot(2,1,2)
        codedbits2 = reshape(reshape(codedbits,3,[])',1,[]);
        plot(abs((codedbits2'>0)-(mibCoded>0)))
        
        %decodedData = lteConvolutionalDecode(noisysymbols);
        %[decodedData2, quant] = srslte_viterbi(interleavedSymbols);
        
        if (nof_ports2 ~= NofPortsTx)
            errorReal(i,2)=1;
        end
    end
    error(snr_idx,:) = sum(errorReal);
    fprintf('SNR: %.2f dB\n', SNR_values_db(snr_idx));
end

if (length(SNR_values_db) > 1)
    semilogy(SNR_values_db, error/Nrealizations)
    grid on
    xlabel('SNR (dB)');
    ylabel('BLER')
    legend('Matlab','srsLTE')
    axis([min(SNR_values_db) max(SNR_values_db) 1/Nrealizations/10 1])
else
    disp(error)
    disp(nfmod4)
    disp(mod2)
end

