%% LTE Downlink Channel Estimation and Equalization

%% Cell-Wide Settings

clear

plot_noise_estimation_only=false;

SNR_values_db=100;%linspace(0,30,5);
Nrealizations=1;

w1=1/3;

%% UE Configuration
ue = lteRMCUL('A3-5');
ue.TotSubframes = 2;

K=ue.NULRB*12;
P=K/6;

%% Channel Model Configuration
chs.Seed = 1;                  % Random channel seed
chs.InitTime = 0;
chs.NRxAnts = 1;               % 1 receive antenna
chs.DelayProfile = 'EVA';     
chs.DopplerFreq = 300;          % 120Hz Doppler frequency
chs.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
chs.NTerms = 16;               % Oscillators used in fading model
chs.ModelType = 'GMEDS';       % Rayleigh fading model type
chs.InitPhase = 'Random';      % Random initial phases
chs.NormalizePathGains = 'On'; % Normalize delay profile power 
chs.NormalizeTxAnts = 'On';    % Normalize for transmit antennas

%% Channel Estimator Configuration
cec = struct;                        % Channel estimation config structure
cec.PilotAverage = 'UserDefined';    % Type of pilot symbol averaging
cec.FreqWindow = 9;                 % Frequency window size
cec.TimeWindow = 9;                 % Time window size
cec.InterpType = 'Linear';            % 2D interpolation type
cec.InterpWindow = 'Causal';       % Interpolation window type
cec.InterpWinSize = 1;               % Interpolation window size


%% Allocate memory
Ntests=3;
hest=cell(1,Ntests);
for i=1:Ntests
    hest{i}=zeros(K,14);
end
MSE=zeros(Ntests,Nrealizations,length(SNR_values_db));
noiseEst=zeros(Ntests,Nrealizations,length(SNR_values_db));

legends={'matlab','ls',num2str(w1)};
colors={'bo-','rx-','m*-','k+-','c+-'};
colors2={'b-','r-','m-','k-','c-'};

addpath('../../debug/srslte/lib/ch_estimation/test')

offset = -1; 
for nreal=1:Nrealizations

    %% Signal Generation
    [txWaveform, txGrid, info] = lteRMCULTool(ue,[1;0;0;1]);

    %% SNR Configuration
    for snr_idx=1:length(SNR_values_db)
        SNRdB = SNR_values_db(snr_idx);             % Desired SNR in dB
        SNR = 10^(SNRdB/20);    % Linear SNR  

        fprintf('SNR=%.1f dB\n',SNRdB)

        %% Fading Channel

        chs.SamplingRate = info.SamplingRate;
        [rxWaveform, chinfo] = lteFadingChannel(chs,txWaveform);
        
        %% Additive Noise

        % Calculate noise gain
        N0 = 1/(sqrt(2.0*double(info.Nfft))*SNR);

        % Create additive white Gaussian noise
        noise = N0*complex(randn(size(rxWaveform)),randn(size(rxWaveform)));   

        % Add noise to the received time domain waveform
        rxWaveform = rxWaveform + noise;

        %% Synchronization

        % Time offset estimation is done once because depends on channel
        % model only
        if (offset==-1) 
            offset = lteULFrameOffset(ue,ue.PUSCH,rxWaveform);
        end
        rxWaveform = rxWaveform(1+offset:end);
        
        %% OFDM Demodulation
        rxGrid = lteSCFDMADemodulate(ue,rxWaveform);
        rxGrid = rxGrid(:,1:14);

        %% Perfect channel estimate
        h=lteULPerfectChannelEstimate(ue,chs,offset); 
        h=h(:,1:14);
        
        %% Channel Estimation with Matlab
        [hest{1}, noiseEst(1,nreal,snr_idx)] = lteULChannelEstimate(ue,ue.PUSCH,cec,rxGrid);

        %% LS-Linear estimation with srsLTE
        [hest{2}, noiseEst(2,nreal,snr_idx)] = srslte_chest_ul(ue,ue.PUSCH,rxGrid);
        
        %% LS-Linear estimation + averaging with srsLTE
        [hest{3}, noiseEst(3,nreal,snr_idx)] = srslte_chest_ul(ue,ue.PUSCH,rxGrid,w1);
        
        %% Compute MSE 
        for i=1:Ntests
            MSE(i,nreal,snr_idx)=mean(mean(abs(h-hest{i}).^2));
            fprintf('MSE test %d: %f\n',i, 10*log10(MSE(i,nreal,snr_idx)));
        end

        %% Plot a single realization
        if (length(SNR_values_db) == 1)
            subplot(1,1,1)
            sym=1;
            n=1:(K*length(sym));
            for i=1:Ntests
                plot(n,abs(reshape(hest{i}(:,sym),1,[])),colors2{i});
                hold on;
            end
            plot(n,abs(reshape(h(:,sym),1,[])),'k');
            hold off;
            
            tmp=cell(Ntests+1,1);
            for i=1:Ntests
                tmp{i}=legends{i};
            end
            tmp{Ntests+1}='Perfect';
            legend(tmp)

            xlabel('SNR (dB)')
            ylabel('Channel Gain')
            grid on;

        end

    end
end

%% Compute average MSE and noise estimation
mean_mse=mean(MSE,2);
mean_snr=10*log10(1./mean(noiseEst,2));

%disp(mean_snr(3)

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

