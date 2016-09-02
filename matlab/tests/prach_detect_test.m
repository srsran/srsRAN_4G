%% PRACH Detection Conformance Test
%clear

d=50;%linspace(4,14,6);
pDetection2 = zeros(2,length(d));
for dd=1:length(d)
detect_factor=d(dd);

numSubframes = 1;  % Number of subframes frames to simulate at each SNR
SNRdB = 10;%linspace(-14,10,8);  % SNR points to simulate
foffset = 0.0;           % Frequency offset in Hertz
delay=0;
add_fading=false; 

addpath('../../build/srslte/lib/phch/test')

%% UE Configuration
% User Equipment (UE) settings are specified in the structure |ue|.

ue.NULRB = 15;                   % 6 Resource Blocks
ue.DuplexMode = 'FDD';          % Frequency Division Duplexing (FDD)
ue.CyclicPrefixUL = 'Normal';   % Normal cyclic prefix length
ue.NTxAnts = 1;                 % Number of transmission antennas
    
%% PRACH Configuration

prach.Format = 0;          % PRACH format: TS36.104, Table 8.4.2.1-1
prach.HighSpeed = 0;       % Normal mode: TS36.104, Table 8.4.2.1-1
prach.FreqOffset = 4;      % Default frequency location
info = ltePRACHInfo(ue, prach);  % PRACH information
    
%% Propagation Channel Configuration
% Configure the propagation channel model using a structure |chcfg| as per
% TS36.104, Table 8.4.2.1-1 [ <#9 1> ].

chcfg.NRxAnts = 1;                       % Number of receive antenna
chcfg.DelayProfile = 'ETU';              % Delay profile
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

        prach.SeqIdx = 0;%randi(838,1,1)-1;         % Logical sequence index: TS36.141, Table A.6-1
        prach.CyclicShiftIdx = 11;%randi(16,1,1)-1;  % Cyclic shift index: TS36.141, Table A.6-1
        prach.PreambleIdx = 1;%randi(64,1,1)-1;    % Preamble index: TS36.141, Table A.6-1
        info = ltePRACHInfo(ue, prach);  % PRACH information

        % PRACH transmission
        ue.NSubframe = mod(nsf-1, 10);
        ue.NFrame = fix((nsf-1)/10);
        
        % Set PRACH timing offset in us as per TS36.141, Figure 8.4.1.4.2-2
        %prach.TimingOffset = info.BaseOffset + ue.NSubframe/10.0;
        prach.TimingOffset = 0;
        
        % Generate transmit wave
        [txwave,prachinfo] = ltePRACH(ue, prach);             

        % Channel modeling
        if (add_fading) 
            chcfg.InitTime = (nsf-1)/1000;
            [rxwave, fadinginfo] = lteFadingChannel(chcfg, ...
                                [txwave; zeros(25, 1)]);
        else 
            rxwave = txwave;
        end
        % Add noise
        %noise = N*complex(randn(size(rxwave)), randn(size(rxwave)));            
        %rxwave = rxwave + noise;            
        % Remove the implementation delay of the channel modeling
        if (add_fading)
            rxwave = rxwave((fadinginfo.ChannelFilterDelay + 1):end, :);  
        end

        rxwave=x;
       % rxwave=[zeros(delay,1); txwave(1:end-delay)];

        % Apply frequency offset
        t = ((0:size(rxwave, 1)-1)/chcfg.SamplingRate).';
        rxwave = rxwave .* repmat(exp(1i*2*pi*foffset*t), ...
            1, size(rxwave, 2));
        

        % PRACH detection for all cell preamble indices
        [detected, offsets] = ltePRACHDetect(ue, prach, rxwave, (0:63).');
        
        [detected_srs, offsets_srs, corrout] = srslte_prach_detect(ue, prach, rxwave, detect_factor);
        
        disp(detected)
        disp(detected_srs)
        disp(offsets_srs*1e6)
        
        % Test for preamble detection
        if (length(detected)==1)
            
            % Test for correct preamble detection
            if (detected==prach.PreambleIdx)         
                
                % Calculate timing estimation error. The true offset is
                % PRACH offset plus channel delay
                trueOffset = prach.TimingOffset/1e6 + 310e-9;
                measuredOffset = offsets(1)/chcfg.SamplingRate;
                timingerror = abs(measuredOffset-trueOffset);
                
                % Test for acceptable timing error
                if (timingerror<=2.08e-6)
                    detectedCount = detectedCount + 1; % Detected preamble
                else
                    disp('Timing error');
                end
            else
                disp('Detected incorrect preamble');
            end
        else
            disp('Detected multiple or zero preambles');
        end
        
        % Test for preamble detection
        if (length(detected_srs)==1)
            
            % Test for correct preamble detection
            if (detected_srs==prach.PreambleIdx)         
                
                % Calculate timing estimation error. The true offset is
                % PRACH offset plus channel delay
                trueOffset = prach.TimingOffset/1e6 + 310e-9;
                measuredOffset = offsets_srs(1);
                timingerror = abs(measuredOffset-trueOffset);
                
                % Test for acceptable timing error
                if (timingerror<=2.08e-6)
                    detectedCount_srs = detectedCount_srs + 1; % Detected preamble
                else
                    disp('SRS: Timing error');
                end
            else
                disp('SRS: Detected incorrect preamble');
            end
        else
            fprintf('SRS: Detected %d preambles. D=%.1f, Seq=%3d, NCS=%2d, Idx=%2d\n', ...
                length(detected_srs),detect_factor, prach.SeqIdx, prach.CyclicShiftIdx, prach.PreambleIdx);
        end
            

    end % of subframe loop

    % Compute final detection probability for this SNR
    pDetection(1,nSNR) = detectedCount/numSubframes;
    pDetection(2,nSNR) = detectedCount_srs/numSubframes;

end % of SNR loop

pDetection2(1,dd)=pDetection(1,1);
pDetection2(2,dd)=pDetection(2,1);
end
%% Analysis
if (length(SNRdB)>1)
    plot(SNRdB, pDetection)
    legend('Matlab','srsLTE')
    grid on
    xlabel('SNR (dB)')
    ylabel('Pdet')
else
    plot(d,pDetection2)
    legend('Matlab','srsLTE')
    grid on
    xlabel('d')
    ylabel('Pdet')
    fprintf('Pdet=%.4f%%, Pdet_srs=%.4f%%\n',pDetection(1,nSNR),pDetection(2,nSNR))
end

plot(corrout)
