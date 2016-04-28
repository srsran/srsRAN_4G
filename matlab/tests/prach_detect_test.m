%% PRACH Detection Conformance Test
clear

numSubframes = 50;  % Number of subframes frames to simulate at each SNR
SNRdB = linspace(-12,-4,7);  % SNR points to simulate
foffset = 270.0;                        % Frequency offset in Hertz

%% UE Configuration
% User Equipment (UE) settings are specified in the structure |ue|.

ue.NULRB = 6;                   % 6 Resource Blocks
ue.DuplexMode = 'FDD';          % Frequency Division Duplexing (FDD)
ue.CyclicPrefixUL = 'Normal';   % Normal cyclic prefix length
ue.NTxAnts = 1;                 % Number of transmission antennas
    
%% PRACH Configuration

prach.Format = 0;          % PRACH format: TS36.104, Table 8.4.2.1-1
prach.SeqIdx = 22;         % Logical sequence index: TS36.141, Table A.6-1
prach.CyclicShiftIdx = 1;  % Cyclic shift index: TS36.141, Table A.6-1
prach.HighSpeed = 0;       % Normal mode: TS36.104, Table 8.4.2.1-1
prach.FreqOffset = 0;      % Default frequency location
prach.PreambleIdx = 32;    % Preamble index: TS36.141, Table A.6-1

info = ltePRACHInfo(ue, prach);  % PRACH information
    
%% Propagation Channel Configuration
% Configure the propagation channel model using a structure |chcfg| as per
% TS36.104, Table 8.4.2.1-1 [ <#9 1> ].

chcfg.NRxAnts = 1;                       % Number of receive antenna
chcfg.DelayProfile = 'EPA';              % Delay profile
chcfg.DopplerFreq = 70.0;                % Doppler frequency  
chcfg.MIMOCorrelation = 'Low';           % MIMO correlation
chcfg.Seed = 1;                          % Channel seed   
chcfg.NTerms = 16;                       % Oscillators used in fading model
chcfg.ModelType = 'GMEDS';               % Rayleigh fading model type 
chcfg.InitPhase = 'Random';              % Random initial phases        
chcfg.NormalizePathGains = 'On';         % Normalize delay profile power
chcfg.NormalizeTxAnts = 'On';            % Normalize for transmit antennas
chcfg.SamplingRate = info.SamplingRate;  % Sampling rate
    
%% Loop for SNR Values

% Initialize the random number generator stream
rng('default');
    
% Initialize variables storing probability of detection at each SNR
pDetection = zeros(2,length(SNRdB));

for nSNR = 1:length(SNRdB)

    % Scale noise to ensure the desired SNR after SC-FDMA demodulation
    ulinfo = lteSCFDMAInfo(ue);
    SNR = 10^(SNRdB(nSNR)/20);        
    N = 1/(SNR*sqrt(double(ulinfo.Nfft)))/sqrt(2.0); 

    % Detected preamble count
    detectedCount = 0;  
    detectedCount_srs = 0; 
    
    % Loop for each subframe
    for nsf = 1:numSubframes

        % PRACH transmission
        ue.NSubframe = mod(nsf-1, 10);
        ue.NFrame = fix((nsf-1)/10);
        
        % Set PRACH timing offset in us as per TS36.141, Figure 8.4.1.4.2-2
        prach.TimingOffset = info.BaseOffset + ue.NSubframe/10.0;
        prach.TimingOffset = 0; 
        
        % Generate transmit wave
        txwave = ltePRACH(ue, prach);             

        % Channel modeling
        chcfg.InitTime = (nsf-1)/1000;
        %[rxwave, fadinginfo] = lteFadingChannel(chcfg, ...
        %                        [txwave; zeros(25, 1)]);

        rxwave=txwave;
        
        % Add noise
        noise = N*complex(randn(size(rxwave)), randn(size(rxwave)));            
        rxwave = rxwave + noise;            

        % Remove the implementation delay of the channel modeling
        %rxwave = rxwave((fadinginfo.ChannelFilterDelay + 1):1920, :);  

        % Apply frequency offset
        %t = ((0:size(rxwave, 1)-1)/chcfg.SamplingRate).';
        %rxwave = rxwave .* repmat(exp(1i*2*pi*foffset*t), ...
        %    1, size(rxwave, 2));

        % PRACH detection for all cell preamble indices
        [detected, offsets] = ltePRACHDetect(ue, prach, rxwave, (0:63).');
        
        txwave_srs = srslte_prach(ue, prach);
        plot(abs(txwave-txwave_srs))
        
        [detected_srs] = srslte_prach_detect(ue, prach, rxwave);
        
        
        % Test for preamble detection
        if (length(detected)==1)
            
            % Test for correct preamble detection
            if (detected==prach.PreambleIdx)         
                detectedCount = detectedCount + 1; % Detected preamble
                
                % Calculate timing estimation error. The true offset is
                % PRACH offset plus channel delay
%                 trueOffset = prach.TimingOffset/1e6 + 310e-9;
%                 measuredOffset = offsets(1)/chcfg.SamplingRate;
%                 timingerror = abs(measuredOffset-trueOffset);
%                 
%                 % Test for acceptable timing error
%                 if (timingerror<=2.08e-6)
%                     detectedCount = detectedCount + 1; % Detected preamble
%                 else
%                     disp('Timing error');
%                 end
            else
                disp('Detected incorrect preamble');
            end
        else
            disp('Detected multiple or zero preambles');
        end
        
        if (length(detected_srs)==1 && detected_srs==prach.PreambleIdx)
            detectedCount_srs = detectedCount_srs + 1;
        end
            

    end % of subframe loop

    % Compute final detection probability for this SNR
    pDetection(1,nSNR) = detectedCount/numSubframes;
    pDetection(2,nSNR) = detectedCount_srs/numSubframes;

end % of SNR loop

%% Analysis

plot(SNRdB, pDetection)
legend('Matlab','srsLTE')
grid on
xlabel('SNR (dB)')
ylabel('Pdet')


