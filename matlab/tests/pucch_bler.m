clear 
ueConfig=struct('NCellID',1,'RNTI',11,'NULRB',25,'NSubframe',0,'CyclicPrefixUL','Normal','NTxAnts',1,'Hopping','Off');
pucchConfig=struct('NLayers',1,'OrthCover','Off','Shortened',0,'ResourceSize',2);

format_str={'1','1a'};

threshold=[0.5 0];
formats=[1];
pucchConfig.ResourceIdx= 0;
pucchConfig.DeltaShift = 1;
pucchConfig.CyclicShifts = 0;
ueConfig.NSubframe=0;

enable_fading=false; 

SNR_values=-5;%linspace(-8,0,8);
Nreal=50;

% Setup Fading channel model 
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

% Setup matlab channel equalizer
cec.PilotAverage = 'UserDefined';     % Type of pilot averaging
cec.FreqWindow = 9;                   % Frequency window size
cec.TimeWindow = 9;                   % Time window size
cec.InterpType = 'linear';             % 2D interpolation type
cec.InterpWindow = 'Causal';        % Interpolation window type
cec.InterpWinSize = 1;                % Interpolation window size


addpath('../../debug/srslte/lib/phch/test')


ber=zeros(length(formats),length(SNR_values));
ber2=zeros(length(formats),length(SNR_values));
for f=1:length(formats)
    nb=formats(f);
    for s=1:length(SNR_values)
        SNRdB=SNR_values(s);
        SNR = 10^(SNRdB/10);    % Linear SNR  

        errors = 0;
        errors2 = 0;
        for n=1:Nreal
            bits=randi(2,nb-1,1)-1;

            [sym_mat, info]=ltePUCCH1(ueConfig,pucchConfig,bits);
            idx=ltePUCCH1Indices(ueConfig,pucchConfig);
            [dmrs_mat, info_dmrs]=ltePUCCH1DRS(ueConfig,pucchConfig);
            idx_dmrs=ltePUCCH1DRSIndices(ueConfig,pucchConfig);                                        

            % Resource mapping
            subframe_tx = lteULResourceGrid(ueConfig);
            subframe_tx(idx)=sym_mat;
            subframe_tx(idx_dmrs)=dmrs_mat;

            [txWaveform, info] = lteSCFDMAModulate(ueConfig,subframe_tx);
            cfg.SamplingRate = info.SamplingRate;

            % Fading
            if (enable_fading)
                rxWaveform = lteFadingChannel(cfg,txWaveform);
            else
                rxWaveform = txWaveform;
            end
            
            % Noise Addition
            N0  = 1/(sqrt(2.0*double(info.Nfft))*SNR);
            noise = N0*complex(randn(size(rxWaveform)), randn(size(rxWaveform)));  % Generate noise
            rxWaveform = rxWaveform + noise; 

            % Demodulate 
            subframe_rx = lteSCFDMADemodulate(ueConfig, rxWaveform);

            % Perform channel estimation
            [hest, nest] = lteULChannelEstimatePUCCH1(ueConfig, pucchConfig, cec, subframe_rx);

            % Equalize
            pucchSymbols = lteEqualizeMMSE(subframe_rx(idx), hest(idx), nest);

            % Decoding 
            bits_rx = ltePUCCH1Decode(ueConfig, pucchConfig, length(bits), pucchSymbols);

            % Check errors
            a=size(bits_rx);
            if (a(2) ~= 1)
                errors = errors + 1; 
            elseif (formats(f) == 2)
                if (a(1) ~= 1)
                    errors = errors + 1;
                elseif (bits_rx(1) ~= bits(1))
                    errors = errors + 1; 
                end
            end
            
            % Decoding srsLTE
            [bits_rx,z,ce]= srslte_pucch(ueConfig, pucchConfig, length(bits), subframe_rx, threshold);

            % Check errors
            a=size(bits_rx);
            if (a(2) ~= 1)
                errors2 = errors2 + 1; 
            elseif (formats(f) == 2)
                if (a(1) ~= 1)
                    errors2 = errors2 + 1;
                elseif (bits_rx(1) ~= bits(1))
                    errors2 = errors2 + 1; 
                end
            end

        end
        ber(f,s)=errors/Nreal;
        ber2(f,s)=errors2/Nreal;
        fprintf('Format %s, SNR=%.1f dB, errors=%d, errors2=%d\n', format_str{formats(f)},SNRdB,errors,errors2);
    end
end

semilogy(SNR_values,ber,SNR_values,ber2)
xlabel('SNR (dB)')
ylabel('BER')
grid on
legend(format_str(formats))

