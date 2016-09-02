
%% PDSCH decoding based on RMC channels

%% Cell-Wide Settings
% A structure |enbConfig| is used to configure the eNodeB.
%clear12

recordedSignal=[];

Npackets = 1;
SNR_values = 56;%linspace(2,6,10);

Lp=12;
N=256;
K=180;
rstart=(N-K)/2;
P=K/6;
Rhphp=zeros(P,P);
Rhhp=zeros(K,P);
Rhh=zeros(K,K);

t=0:Lp-1;
alfa=log(2*Lp)/Lp;
c_l=exp(-t*alfa);
c_l=c_l/sum(c_l);
C_l=diag(1./c_l);
prows=rstart+(1:6:K);

F=dftmtx(N);
F_p=F(prows,1:Lp);
F_l=F((rstart+1):(K+rstart),1:Lp);
Wi=(F_p'*F_p+C_l*0.01)^(-1);
W2=F_l*Wi*F_p';
w2=reshape(transpose(W2),1,[]);


%% Choose RMC 
[waveform,rgrid,rmccFgOut] = lteRMCDLTool('R.5',[1;0;0;1]);
waveform = sum(waveform,2);

if ~isempty(recordedSignal)
    rmccFgOut = struct('CellRefP',1,'NDLRB',25,'DuplexMode','FDD','CyclicPrefix','Normal'); 
    rmccFgOut.PDSCH.RNTI = 1234;
    rmccFgOut.PDSCH.PRBSet = repmat(transpose(0:rmccFgOut.NDLRB-1),1,2);
    rmccFgOut.PDSCH.TxScheme = 'Port0';
    rmccFgOut.PDSCH.NLayers = 1;   
    rmccFgOut.PDSCH.NTurboDecIts = 5;
    rmccFgOut.PDSCH.Modulation = {'64QAM'};
    trblklen=75376;
    rmccFgOut.PDSCH.TrBlkSizes = trblklen*ones(10,1);
    rmccFgOut.PDSCH.RV = 0;
end

flen=rmccFgOut.SamplingRate/1000;
    
Nsf = 2; 

%% Setup Fading channel model 
cfg.Seed = 0;                  % Random channel seed
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
cfg.SamplingRate = rmccFgOut.SamplingRate; 

% Setup channel equalizer
cec.PilotAverage = 'UserDefined';     % Type of pilot averaging
cec.FreqWindow = 1;                   % Frequency window size
cec.TimeWindow = 1;                   % Time window size
cec.InterpType = 'linear';             % 2D interpolation type
cec.InterpWindow = 'Causal';        % Interpolation window type
cec.InterpWinSize = 1;                % Interpolation window size

addpath('../../build/srslte/lib/phch/test')

decoded = zeros(size(SNR_values));
decoded_srslte = zeros(size(SNR_values));

for snr_idx=1:length(SNR_values)
    SNRdB = SNR_values(snr_idx);
    SNR = 10^(SNRdB/10);    % Linear SNR  
    N0  = 1/(sqrt(2.0*rmccFgOut.CellRefP*double(rmccFgOut.Nfft))*SNR);
    
    Rhphp=zeros(30,30);
    Rhhp=zeros(180,30);

    for i=1:Npackets

        if isempty(recordedSignal)

            %% Fading
            [rxWaveform, chinfo] = lteFadingChannel(cfg,waveform);
            rxWaveform = rxWaveform(chinfo.ChannelFilterDelay+1:end);
            %rxWaveform = waveform; 
            
            %% Noise Addition
            noise = N0*complex(randn(size(rxWaveform)), randn(size(rxWaveform)));  % Generate noise
            rxWaveform = rxWaveform + noise; 
        else        
            rxWaveform = recordedSignal; 
        end
        
        %% Demodulate 
        frame_rx = lteOFDMDemodulate(rmccFgOut, rxWaveform);

        for sf_idx=0:Nsf-1
       % sf_idx=9;
            subframe_rx=frame_rx(:,sf_idx*14+1:(sf_idx+1)*14);
            rmccFgOut.NSubframe=sf_idx;
            rmccFgOut.TotSubframes=1;

            % Perform channel estimation
            [hest, nest] = lteDLChannelEstimate(rmccFgOut, cec, subframe_rx);

            [cws,symbols] = ltePDSCHDecode(rmccFgOut,rmccFgOut.PDSCH,subframe_rx,hest,nest);
            [trblkout,blkcrc,dstate] = lteDLSCHDecode(rmccFgOut,rmccFgOut.PDSCH, ... 
                                                    rmccFgOut.PDSCH.TrBlkSizes(sf_idx+1),cws);

            decoded(snr_idx) = decoded(snr_idx) + ~blkcrc;


            %% Same with srsLTE
            if (rmccFgOut.PDSCH.TrBlkSizes(sf_idx+1) > 0)
                [dec2, data, pdschRx, pdschSymbols2, cws2, ce] = srslte_pdsch(rmccFgOut, rmccFgOut.PDSCH, ... 
                                                        rmccFgOut.PDSCH.TrBlkSizes(sf_idx+1), ...
                                                        subframe_rx);
            else
                dec2 = 1;
            end
             if (~dec2) 
                fprintf('Error in sf=%d\n',sf_idx);
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
    semilogy(SNR_values,1-decoded/Npackets/(Nsf),'bo-',...
             SNR_values,1-decoded_srslte/Npackets/(Nsf), 'ro-')
    grid on;
    legend('Matlab','srsLTE')
    xlabel('SNR (dB)')
    ylabel('BLER')
    axis([min(SNR_values) max(SNR_values) 1/Npackets/(Nsf+1) 1])
else
    subplot(2,1,1)
    scatter(real(pdschSymbols2),imag(pdschSymbols2))
    %plot(real(hest))
    subplot(2,1,2)
    %plot(1:180,angle(ce(1:180)),1:180,angle(hest(:,1)))
    plot(abs(ce-hest(:)))
    fprintf('Matlab: %d OK\nsrsLTE: %d OK\n',decoded, decoded_srslte);
end

