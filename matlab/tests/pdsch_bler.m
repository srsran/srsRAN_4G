%% PDSCH decoding based on RMC channels

%% Cell-Wide Settings
% A structure |enbConfig| is used to configure the eNodeB.
%clear

recordedSignal=[];

Npackets = 1;
SNR_values = linspace(15,20,4);

%% Choose RMC 
[waveform,rgrid,rmccFgOut] = lteRMCDLTool('R.4',[1;0;0;1]);
waveform = sum(waveform,2);

if ~isempty(recordedSignal)
    rmccFgOut = struct('NCellID',1,'CellRefP',1,'CFI',1,'NDLRB',15,'SamplingRate',3.84e6,'Nfft',256,'DuplexMode','FDD','CyclicPrefix','Normal'); 
    rmccFgOut.PDSCH.RNTI = 1234;
    rmccFgOut.PDSCH.PRBSet = repmat(transpose(0:rmccFgOut.NDLRB-1),1,2);
    rmccFgOut.PDSCH.TxScheme = 'Port0';
    rmccFgOut.PDSCH.NLayers = 1;   
    rmccFgOut.PDSCH.NTurboDecIts = 5;
    rmccFgOut.PDSCH.Modulation = {'64QAM'};
    rmccFgOut.PDSCH.TrBlkSizes = [0 5992*ones(1,4) 0 5992*ones(1,4)];
    rmccFgOut.PDSCH.RV = 0;
end

flen=rmccFgOut.SamplingRate/1000;
    
Nsf = 9; 

%% Setup Fading channel model 
cfg.Seed = 8;                  % Random channel seed
cfg.NRxAnts = 1;               % 1 receive antenna
cfg.DelayProfile = 'EVA';      % EVA delay spread
cfg.DopplerFreq = 5;           % 120Hz Doppler frequency
cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
cfg.InitTime = 0;              % Initialize at time zero
cfg.NTerms = 16;               % Oscillators used in fading model
cfg.ModelType = 'GMEDS';       % Rayleigh fading model type
cfg.InitPhase = 'Random';      % Random initial phases
cfg.NormalizePathGains = 'On'; % Normalize delay profile power 
cfg.NormalizeTxAnts = 'On';    % Normalize for transmit antennas
cfg.SamplingRate = rmccFgOut.SamplingRate; 

% Setup channel equalizer
cec.PilotAverage = 'UserDefined';     % Type of pilot averaging
cec.FreqWindow = 9;                   % Frequency window size
cec.TimeWindow = 9;                   % Time window size
cec.InterpType = 'linear';             % 2D interpolation type
cec.InterpWindow = 'Centered';        % Interpolation window type
cec.InterpWinSize = 1;                % Interpolation window size

addpath('../../build/srslte/lib/phch/test')

decoded = zeros(size(SNR_values));
decoded_srslte = zeros(size(SNR_values));

for snr_idx=1:length(SNR_values)
    SNRdB = SNR_values(snr_idx);
    SNR = 10^(SNRdB/10);    % Linear SNR  
    N0  = 1/(sqrt(2.0*rmccFgOut.CellRefP*double(rmccFgOut.Nfft))*SNR);
    for i=1:Npackets

        if isempty(recordedSignal)

            %% Fading
            rxWaveform = lteFadingChannel(cfg,waveform);
            %rxWaveform = waveform; 
            
            %% Noise Addition
            noise = N0*complex(randn(size(rxWaveform)), randn(size(rxWaveform)));  % Generate noise
            rxWaveform = rxWaveform + noise; 
        else        
            rxWaveform = recordedSignal; 
        end
        
        %% Demodulate 
        frame_rx = lteOFDMDemodulate(rmccFgOut, rxWaveform);

        for sf_idx=0:Nsf
            subframe_waveform = rxWaveform(sf_idx*flen+1:(sf_idx+1)*flen);
            subframe_rx=frame_rx(:,sf_idx*14+1:(sf_idx+1)*14);
            rmccFgOut.NSubframe=sf_idx;
            rmccFgOut.TotSubframes=1;

            % Perform channel estimation
            [hest, nest] = lteDLChannelEstimate(rmccFgOut, cec, subframe_rx);

            [cws,symbols,pdschSymbols,hestCH,indices] = ltePDSCHDecode2(rmccFgOut,rmccFgOut.PDSCH,subframe_rx,hest,nest);
            [trblkout,blkcrc,dstate] = lteDLSCHDecode(rmccFgOut,rmccFgOut.PDSCH, ... 
                                                    rmccFgOut.PDSCH.TrBlkSizes(sf_idx+1),cws);

            decoded(snr_idx) = decoded(snr_idx) + ~blkcrc;


            %% Same with srsLTE
            if (rmccFgOut.PDSCH.TrBlkSizes(sf_idx+1) > 0)
                [dec2, data, pdschRx, pdschSymbols2, deb] = srslte_pdsch(rmccFgOut, rmccFgOut.PDSCH, ... 
                                                        rmccFgOut.PDSCH.TrBlkSizes(sf_idx+1), ...
                                                        subframe_waveform);
            else
                dec2 = 1;
            end
            decoded_srslte(snr_idx) = decoded_srslte(snr_idx)+dec2;
        end

        if ~isempty(recordedSignal)
            recordedSignal = recordedSignal(flen*10+1:end);
        end
    end
    fprintf('SNR: %.1f. Decoded: %d-%d\n',SNRdB, decoded(snr_idx), decoded_srslte(snr_idx))
end

if (length(SNR_values)>1)
    semilogy(SNR_values,1-decoded/Npackets/(Nsf+1),'bo-',...
             SNR_values,1-decoded_srslte/Npackets/(Nsf+1), 'ro-')
    grid on;
    legend('Matlab','srsLTE')
    xlabel('SNR (dB)')
    ylabel('BLER')
    axis([min(SNR_values) max(SNR_values) 1/Npackets/(Nsf+1) 1])
else
    disp(decoded)
    disp(decoded_srslte)
end

