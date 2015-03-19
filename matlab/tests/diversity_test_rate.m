%%  PDSCH Transmit Diversity Throughput Conformance Test
% This example demonstrates the throughput performance under conformance 
% test conditions as defined in TS36.101[ <#9 1> ]: single codeword, 
% transmit diversity 4Tx-2Rx with medium correlation, EPA5 (Extended 
% Pedestrian A) channel. The example also introduces the use of Parallel
% Computing Toolbox(TM) to provide improvements in the simulation time.

% Copyright 2009-2013 The MathWorks, Inc.

%% Introduction
% In this example, Hybrid Automatic Repeat Request(HARQ) is used in line
% with conformance test requirements. A total of 8 HARQ processes are used
% with a maximum of 4 retransmissions permitted. This example uses the R.12
% Reference Measurement Channel (RMC).
% 
% This example also uses <matlab:doc('parfor') parfor> loop instead of the
% <matlab:doc('for') for> loop for SNR calculation. <matlab:doc('parfor')
% parfor>, as part of the Parallel Computing Toolbox, executes the SNR loop
% in parallel to reduce the total simulation time.

%% Simulation Settings
% The default simulation length is set to 10 frames at a number of |SNR|
% values including 0.2dB (as per TS36.101 (Section 8.2.1.2.2-Test1)[ <#9
% 1> ]).
NFrames = 50;                        % Number of frames
SNRdB = [4 5 6];         % SNR range

% eNodeB Configuration
enb = struct;                        % eNodeB config structure
enb.TotSubframes = 1;                % Total subframes RMC will generate
enb.RC = 'R.10';                     % RMC number

% Channel Configuration
channel = struct;                    % Channel config structure
channel.Seed = 2;                    % Random channel seed
channel.NRxAnts = 1;                 % 2 receive antennas
channel.DelayProfile ='EVA';         % Delay profile
channel.DopplerFreq = 5;             % Doppler frequency
channel.MIMOCorrelation = 'Low';  % Multi-antenna correlation
channel.NTerms = 16;                 % Oscillators used in fading model
channel.ModelType = 'GMEDS';         % Rayleigh fading model type
channel.InitPhase = 'Random';        % Random initial phases
channel.NormalizePathGains = 'On';   % Normalize delay profile power 
channel.NormalizeTxAnts = 'On';      % Normalize for transmit antennas

% Channel Estimator Configuration
cec = struct;                        % Channel estimation config structure
cec.PilotAverage = 'UserDefined';    % Type of pilot symbol averaging
cec.FreqWindow = 9;                 % Frequency window size
cec.TimeWindow = 9;                 % Time window size
cec.InterpType = 'Linear';            % 2D interpolation type
cec.InterpWindow = 'Centered';       % Interpolation window type
cec.InterpWinSize = 1;               % Interpolation window size

% PDSCH Configuration
enb.PDSCH.TxScheme = 'TxDiversity';  % Transmission scheme
enb.PDSCH.RNTI = 1;                  % 16-bit User Equipment (UE) mask
enb.PDSCH.Rho = 0;                  % PDSCH RE power adjustment factor
enb.PDSCH.CSI = 'Off';               % No CSI scaling of soft bits

% Simulation Variables
totalBLKCRC = [];                    % Define total block CRC vector
bitThroughput = [];                  % Define total bit throughput vector

%% System Processing
% Working on a subframe by subframe basis and using the LTE System
% Toolbox(TM) a populated resource grid is generated and OFDM modulated to
% create a transmit waveform. The generated waveform is transmitted through
% a propagation channel and AWGN is added. Channel estimation, equalization
% and the inverse of transmission chain are performed at receiver. The
% throughput performance of the PDSCH is determined using the block CRC
% result.

% Generate the RMC configuration structure for RMC R.12
rmc = lteRMCDL(enb);
rvSeq = rmc.PDSCH.RVSeq;
    
% Transport block sizes for each subframe in a frame
trBlkSizes = rmc.PDSCH.TrBlkSizes;
codedTrBlkSizes = rmc.PDSCH.CodedTrBlkSizes;
    
% Determine resource grid dimensions
dims = lteDLResourceGridSize(rmc);
p = dims(3);
    
% Set up channel model sampling rate
ofdmInfo = lteOFDMInfo(rmc);
channel.SamplingRate = ofdmInfo.SamplingRate;    

% Generation HARQ table for 8-HARQ processes
harqTable = hHARQTable();

% Initializing state of all HARQ processes
for i=1:9
     harqProcess_init(i) = hTxDiversityNewHARQProcess ...
                    (trBlkSizes(i),codedTrBlkSizes(i),rvSeq); %#ok<SAGROW>
end

% Display the SNR points being simulated
for s=1:numel(SNRdB)
    fprintf('\nSimulating at %gdB SNR for a total %d Frame(s)\n', ...
            SNRdB(s),NFrames);
end

% The temporary variables 'rmc_init' and 'channel_init' are used to create
% the temporary variables 'rmc' and 'channel' within the SNR loop to create
% independent simulation loops for the parfor loop
rmc_init = rmc;
channel_init = channel;

% 'parfor' will default to the normal 'for' when executed without Parallel
% Computing Toolbox. 
parfor index = 1:numel(SNRdB)
    
    % Set the random number generator seed depending to the loop variable
    % to ensure independent random streams
    rng(index,'combRecursive');

    % Set up variables for the SNR loop
    offsets = 0;    % Initialize overall frame offset value for the SNR
    offset = 0;     % Initialize frame offset value for the radio frame
    rmc = rmc_init; % Initialize RMC configuration
    channel = channel_init; % Initialize channel configuration 
    blkCRC = [];    % Define intermediate block CRC vector
    bitTput = [];   % Intermediate bit throughput vector
              
    % Initializing state of all HARQ processes
    harqProcess = harqProcess_init;
    
    for subframeNo = 0:(NFrames*10-1)

        % Updating subframe number
        rmc.NSubframe = subframeNo;

        % HARQ index table
        harqIdx = harqTable(mod(subframeNo,length(harqTable))+1); %#ok<PFBNS>

        % Update HARQ process
        harqProcess(harqIdx) = hTxDiversityHARQScheduling( ...
                               harqProcess(harqIdx)); 

        % Updating the RV value for correct waveform generation
        rmc.PDSCH.RV = harqProcess(harqIdx).rvSeq ...
                       (harqProcess(harqIdx).rvIdx);
                   
        rmc.PDSCH.RVSeq = harqProcess(harqIdx).rvSeq ...
                          (harqProcess(harqIdx).rvIdx);
                      
        [txWaveform txGrid] = lteRMCDLTool(rmc, ...
                   {harqProcess(harqIdx).dlschTransportBlk});
        txWaveform = [txWaveform; zeros(25,p)];

        % Initialize at time zero  
        channel.InitTime = subframeNo/1000;

        % Pass data through the fading channel model 
        rxWaveform = lteFadingChannel(channel,txWaveform);

        % Noise setup including compensation for downlink power allocation
        SNR = 10^((SNRdB(index)-rmc.PDSCH.Rho)/20);    % Linear SNR

        % Normalize noise power to take account of sampling rate, which is
        % a function of the IFFT size used in OFDM modulation, and the 
        % number of antennas
        N0 = 1/(sqrt(2.0*rmc.CellRefP*double(ofdmInfo.Nfft))*SNR); 

        % Create additive white Gaussian noise
        noise = N0*complex(randn(size(rxWaveform)), ...
                            randn(size(rxWaveform)));
                        
        % Add AWGN to the received time domain waveform
        rxWaveform = rxWaveform + noise;

        % Receiver
        % Perform synchronization
        % An offset within the range of delays expected from the channel 
        % modeling(a combination of implementation delay and channel delay
        % spread) indicates success
        if (mod(subframeNo,10)==0)
            [offset] = lteDLFrameOffset(rmc,rxWaveform);
            if (offset > 25)
                offset = offsets(end);
            end
            offsets = [offsets offset]; 
        end
        rxWaveform = rxWaveform(1+offset:end,:);                        

        % Perform OFDM demodulation on the received data to recreate the
        % resource grid
        rxSubframe = lteOFDMDemodulate(rmc,rxWaveform);

        % Equalization and channel estimation
        [estChannelGrid,noiseEst] = lteDLChannelEstimate(rmc,cec, ...
                                                         rxSubframe);

         addpath('../../debug/lte/phy/lib/ch_estimation/test')
        [est, ~, output] = srslte_chest(rmc.NCellID,rmc.CellRefP,rxSubframe,[0.25 0.5 0.25],[0.1 0.9],mod(rmc.NSubframe,10));
        
        %estChannelGrid=reshape(est,size(estChannelGrid));
        
        % Perform deprecoding, layer demapping, demodulation and 
        % descrambling on the received data using the estimate of
        % the channel
        rxEncodedBits = ltePDSCHDecode2(rmc,rmc.PDSCH,rxSubframe,estChannelGrid,noiseEst);

        % Decode DownLink Shared Channel (DL-SCH)
        [decbits,harqProcess(harqIdx).crc,harqProcess(harqIdx).decState] = ...  
            lteDLSCHDecode(rmc,rmc.PDSCH,harqProcess(harqIdx).trBlkSize, ...
            rxEncodedBits{1},harqProcess(harqIdx).decState);
    
        if(harqProcess(harqIdx).trBlkSize ~= 0)
            blkCRC = [blkCRC harqProcess(harqIdx).crc];
            bitTput = [bitTput harqProcess(harqIdx).trBlkSize.*(1- ...
                      harqProcess(harqIdx).crc)];
        end
    end
    % Record the block CRC and bit throughput for the total number of
    % frames simulated at a particular SNR
    totalBLKCRC(index,:) = blkCRC;
    bitThroughput(index,:) = bitTput;

end
%%
% |totalBLKCRC| is a matrix where each row contains the results of decoding
% the block CRC for a defined value of SNR. |bitThroughput| is a matrix
% containing the total number of bits per subframe at the different SNR
% points that have been successfully received and decoded.

%% Results

% First graph shows the throughput as total bits per second against the 
% range of SNRs
% figure;
% plot(SNRdB,mean(bitThroughput,2),'-*');
% %axis([-5 3 200 400])
% title(['Throughput for ', num2str(NFrames) ' frame(s)'] );
% xlabel('SNRdB'); ylabel('Throughput (kbps)');
% grid on;
% hold on;
% plot(SNRdB,mean([trBlkSizes(1:5) trBlkSizes(7:10)])*0.7*ones ...
%     (1,numel(SNRdB)),'--rs');
% legend('Simulation Result','70 Percent Throughput','Location','SouthEast');
% 
% % Second graph shows the total throughput as a percentage of CRC passes 
% % against SNR range
% figure;
plot(SNRdB,100*(1-mean(totalBLKCRC,2)),'-*');
%axis([-5 3 50 110])
title(['Throughput for ', num2str(NFrames) ' frame(s)'] );
xlabel('SNRdB'); ylabel('Throughput (%)');
grid on;
hold on;
plot(SNRdB,70*ones(1,numel(SNRdB)),'--rs');
legend('Simulation Result','70 Percent Throughput','Location','SouthEast');


%% Further Exploration 
% 
% You can modify parts of this example to experiment with different number 
% of |NFrames| and different values of SNR. SNR can be a vector of 
% values or a single value. Following scenarios can be simulated.
%%
% * Allows control over the total number of frames to run the demo at an
% SNR of 0.2dB (as per TS 36.101).
%   
% * Allows control over the total number of frames to run the demo, as well
% as defining a set of desired SNR values. |SNRIn| can be a single value 
% or a vector containing a range of values.
%
% * For simulations of multiple SNR points over a large number of frames,
% the use of Parallel Computing Toolbox provides significant improvement in
% the simulation time. This can be easily verified by changing the |parfor|
% in the SNR loop to |for| and re-running the example.

%% Appendix
% This example uses the following helper functions:
%
% * <matlab:edit('hHARQTable.m') hHARQTable.m>
% * <matlab:edit('hTxDiversityHARQScheduling.m') hTxDiversityHARQScheduling.m>
% * <matlab:edit('hTxDiversityNewHARQProcess.m') hTxDiversityNewHARQProcess.m>

%% Selected Bibliography
% # 3GPP TS 36.101

displayEndOfDemoMessage(mfilename) 
