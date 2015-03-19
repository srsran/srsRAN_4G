%clear
% R.1 10 MHz 1 port
% R.10 10 MHz 2 ports
% R.4 1.4 MHz 1 port 
% R.11-2 5 MHz 2 ports
rmc = lteRMCDL('R.10');

NofPortsTx=1;

SNR_values_db=1;%linspace(-8,-2,4);
Nrealizations=5;
enb = struct('NCellID',1,'NDLRB',25,'CellRefP',NofPortsTx,'CyclicPrefix','Normal','DuplexMode','FDD','NSubframe',0);

griddims = lteResourceGridSize(enb); % Resource grid dimensions
L = griddims(2);    
        
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

rmc.PDSCH.Modulation = '16QAM';
[waveform,rgrid,info] = lteRMCDLTool(rmc,[1;0;0;1]);

cfg.SamplingRate = info.SamplingRate;

addpath('../../debug/lte/phy/lib/phch/test')


error=zeros(length(SNR_values_db),2);
for snr_idx=1:length(SNR_values_db)
    SNRdB = SNR_values_db(snr_idx);             % Desired SNR in dB
    SNR = 10^(SNRdB/20);    % Linear SNR  

    errorReal = zeros(Nrealizations,2);
    for i=1:Nrealizations

        rxWaveform = lteFadingChannel(cfg,sum(waveform,2));

        %% Additive Noise
        N0 = 1/(sqrt(2.0*double(enb.CellRefP)*double(info.Nfft))*SNR);

        % Create additive white Gaussian noise
        noise = N0*complex(randn(size(rxWaveform)),randn(size(rxWaveform)));   

        rxWaveform = noise + rxWaveform;
        
        rxWaveform = x((i-1)*76800+1:i*76800); 
        
        % Number of OFDM symbols in a subframe
        % OFDM demodulate signal
        rxgrid = lteOFDMDemodulate(enb, rxWaveform);

        % Perform channel estimation
        [hest, nest] = lteDLChannelEstimate(enb, cec, rxgrid(:,1:L,:));
      
        pbchIndices = ltePBCHIndices(enb);
        [pbchRx, pbchHest] = lteExtractResources( ...
            pbchIndices, rxgrid(:,1:L,:), hest(:,1:L,:,:));

        % Decode PBCH
        [bchBits, pbchSymbols, nfmod4, mib, nof_ports] = ltePBCHDecode(enb, pbchRx, pbchHest, nest);

        if (nof_ports ~= NofPortsTx)
            errorReal(i,1)=1;
        end
        
        [nof_ports2, pbchSymbols2, pbchBits, ce, ce2, pbchRx2, pbchHest2]= srslte_pbch(enb, rxWaveform, hest, nest);
        if (nof_ports2 ~= NofPortsTx)
            errorReal(i,2)=1;
        end
%         if (errorReal(i,1) ~= errorReal(i,2))
%             i=1;
%         end
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
end

