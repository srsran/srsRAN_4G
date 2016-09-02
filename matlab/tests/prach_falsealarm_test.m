%% PRACH False Alarm Probability Conformance Test
clear

detect_factor=5;

ue.NULRB = 6;                   % Number of resource blocks
ue.DuplexMode = 'FDD';          % FDD duplexing mode

%% PRACH Configuration
prach.Format = 0;               % Preamble format
prach.SeqIdx = 2;               % Logical root sequence index
prach.CyclicShiftIdx = 1;       % Cyclic shift configuration index
prach.HighSpeed = 0;            % High speed flag
prach.FreqOffset = 0;           % Use default frequency resource index
prach.PreambleIdx = [];         % Empty since no preamble is transmitted

%% Establish PRACH Generator Output Length for this Configuration
info = ltePRACHInfo(ue, prach);
nSamples = info.SamplingRate*info.TotSubframes*0.001;

%% Loop for Detection in Each Subframe 
numTrials = 2000;
falseCount = 0;         % Initialize false detection counter
falseCount_srs = 0;         % Initialize false detection counter
rng('default');         % Random number generator to default state

runningP=zeros(1, numTrials);
runningP_srs=zeros(1, numTrials);
for nt = 1:numTrials 
    
    % Create noise
    noise = complex(randn(nSamples, 1), randn(nSamples, 1));

    % Attempt detection for all cell preamble indices (0...63)
    [detected,offset] = ltePRACHDetect(ue, prach, noise, 0:63);
    [detected_srs] = srslte_prach_detect(ue, prach, noise, detect_factor);

    % Record false alarm
    if (~isempty(detected))
        falseCount = falseCount+1;        
    end

    if (~isempty(detected_srs))
        falseCount_srs = falseCount_srs+1;        
    end

    % Calculate running false alarm probability
    runningP(nt) = falseCount/nt*100;
    runningP_srs(nt) = falseCount_srs/nt*100;
    
    % Plot information about false alarm (if applicable)
    if (~isempty(detected))
        plot(nt,runningP(nt),'ro','LineWidth',2,'MarkerSize',7);
        hold on;
        text(nt,runningP(nt), sprintf(['Preamble index = %d' ...
            '  \nTiming offset = %0.2f samples  '],detected,offset), ...
            'HorizontalAlignment','right');
    end

    if (~isempty(detected_srs))
        plot(nt,runningP(nt),'mx','LineWidth',2,'MarkerSize',7);
        hold on;
        text(nt,runningP(nt), sprintf(['SRS index = %d' ...
            '  \nTiming offset = %0.2f samples  '],detected,offset), ...
            'HorizontalAlignment','right');
    end

end

%% Compute Final False Alarm Probability

P = falseCount / numTrials;
P_srs = falseCount_srs / numTrials;
plot(1:numTrials,runningP,'b','LineWidth',2);
plot(1:numTrials,runningP_srs,'k','LineWidth',2);
hold off
axis([0 numTrials+1 -0.1 0.2]);
xlabel('Trials');
ylabel('Running false alarm probability (%)');
title('PRACH False Alarm Detection Probability');

fprintf('\nFalse alarm probability = %0.4f%% - %.04f%%\n',P*100, P_srs*100);

