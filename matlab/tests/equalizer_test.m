%% LTE Downlink Channel Estimation and Equalization

%% Cell-Wide Settings

clear

plot_noise_estimation_only=false;

SNR_values_db=linspace(0,30,7);
Nrealizations=4;

Lp=10;
        
N=512;
K=300;
rstart=(N-K)/2;
P=K/6;
Rhphp=zeros(P,P);
Rhhp=zeros(K,P);
Rhh=zeros(K,K);

enb.NDLRB = 25;                 % Number of resource blocks
enb.CellRefP = 1;               % One transmit antenna port
enb.NCellID = 0;                % Cell ID
enb.CyclicPrefix = 'Normal';    % Normal cyclic prefix
enb.DuplexMode = 'FDD';         % FDD

%% Channel Model Configuration
cfg.Seed = 0;                  % Random channel seed
cfg.InitTime = 0;
cfg.NRxAnts = 1;               % 1 receive antenna
cfg.DelayProfile = 'EVA';     

% doppler 5, 70 300

cfg.DopplerFreq = 200;          % 120Hz Doppler frequency
cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
cfg.NTerms = 16;               % Oscillators used in fading model
cfg.ModelType = 'GMEDS';       % Rayleigh fading model type
cfg.InitPhase = 'Random';      % Random initial phases
cfg.NormalizePathGains = 'On'; % Normalize delay profile power 
cfg.NormalizeTxAnts = 'On';    % Normalize for transmit antennas

%% Channel Estimator Configuration
cec = struct;                        % Channel estimation config structure
cec.PilotAverage = 'UserDefined';    % Type of pilot symbol averaging
cec.FreqWindow = 9;                 % Frequency window size
cec.TimeWindow = 9;                 % Time window size
cec.InterpType = 'Linear';            % 2D interpolation type
cec.InterpWindow = 'Causal';       % Interpolation window type
cec.InterpWinSize = 1;               % Interpolation window size

%% Subframe Resource Grid Size

gridsize = lteDLResourceGridSize(enb);
Ks = gridsize(1);    % Number of subcarriers
L = gridsize(2);    % Number of OFDM symbols in one subframe
Ports = gridsize(3);    % Number of transmit antenna ports

%% Allocate memory
Ntests=5;
hest=cell(1,Ntests);
for i=1:Ntests
    hest{i}=zeros(K,140);
end
hls=zeros(4,4*P*10);
MSE=zeros(Ntests,Nrealizations,length(SNR_values_db));
noiseEst=zeros(Ntests,Nrealizations,length(SNR_values_db));

legends={'matlab','ls.linear','mmse','r.mmse','r.mmse2'};
colors={'bo-','rx-','m*-','k+-','c+-'};
colors2={'b-','r-','m-','k-','c-'};

addpath('../../build/srslte/lib/ch_estimation/test')

offset=-1;

for nreal=1:Nrealizations
%% Transmit Resource Grid
txGrid = [];

%% Payload Data Generation
% Number of bits needed is size of resource grid (K*L*P) * number of bits
% per symbol (2 for QPSK)
numberOfBits = Ks*L*Ports*2; 

% Create random bit stream
inputBits = randi([0 1], numberOfBits, 1); 

% Modulate input bits
inputSym = lteSymbolModulate(inputBits,'QPSK');	

%% Frame Generation

% For all subframes within the frame
for sf = 0:10
    
    % Set subframe number
    enb.NSubframe = mod(sf,10);
    
    % Generate empty subframe
    subframe = lteDLResourceGrid(enb);
    
    % Map input symbols to grid
    subframe(:) = inputSym;

    % Generate synchronizing signals
    pssSym = ltePSS(enb);
    sssSym = lteSSS(enb);
    pssInd = ltePSSIndices(enb);
    sssInd = lteSSSIndices(enb);

    % Map synchronizing signals to the grid
    subframe(pssInd) = pssSym;
    subframe(sssInd) = sssSym;

    % Generate cell specific reference signal symbols and indices
    cellRsSym = lteCellRS(enb);
    cellRsInd = lteCellRSIndices(enb);

    % Map cell specific reference signal to grid
    subframe(cellRsInd) = cellRsSym;
    
    % Append subframe to grid to be transmitted
    txGrid = [txGrid subframe]; %#ok
      
end

txGrid([1:5 68:72],6:7) = ones(10,2);

%% OFDM Modulation

[txWaveform,info] = lteOFDMModulate(enb,txGrid); 
txGrid = txGrid(:,1:140);

%% SNR Configuration
for snr_idx=1:length(SNR_values_db)
SNRdB = SNR_values_db(snr_idx);             % Desired SNR in dB
SNR = 10^(SNRdB/20);    % Linear SNR  

fprintf('SNR=%.1f dB\n',SNRdB)

%% Fading Channel

cfg.SamplingRate = info.SamplingRate;
[rxWaveform, chinfo] = lteFadingChannel(cfg,txWaveform);

%% Additive Noise

% Calculate noise gain
N0 = 1/(sqrt(2.0*enb.CellRefP*double(info.Nfft))*SNR);

% Create additive white Gaussian noise
noise = N0*complex(randn(size(rxWaveform)),randn(size(rxWaveform)));   

% Add noise to the received time domain waveform
rxWaveform_nonoise = rxWaveform;
rxWaveform = rxWaveform + noise;

%% Synchronization

if (offset==-1) 
    offset = lteDLFrameOffset(enb,rxWaveform);
end

rxWaveform = rxWaveform(1+offset:end,:);
rxWaveform_nonoise = rxWaveform_nonoise(1+offset:end,:);

%% OFDM Demodulation
rxGrid = lteOFDMDemodulate(enb,rxWaveform);
rxGrid = rxGrid(:,1:140);

rxGrid_nonoise = lteOFDMDemodulate(enb,rxWaveform_nonoise);
rxGrid_nonoise = rxGrid_nonoise(:,1:140);

% True channel
h=rxGrid_nonoise./(txGrid);


%% Channel Estimation with Matlab
tmpnoise=zeros(10,1);
for i=1:10
    enb.NSubframe=i-1;
    [hest{1}(:,(1:14)+(i-1)*14), tmpnoise(i), hls(:,(1:4*P)+(i-1)*4*P)] = ...
                    lteDLChannelEstimate2(enb,cec,rxGrid(:,(i-1)*14+1:i*14));
end
noiseEst(1,nreal,snr_idx)=mean(tmpnoise)*sqrt(2)*enb.CellRefP;

%% LS-Linear estimation with srsLTE
[tmp, ~, ~, noiseEst(2,nreal,snr_idx)] = srslte_chest(enb.NCellID,enb.CellRefP,rxGrid);
hest{2}=reshape(tmp, size(hest{1}));

tmp_noise=zeros(2,1);
for sf=[0 5]
    enb.NSubframe=sf;
    pssSym = ltePSS(enb);
    pssInd = ltePSSIndices(enb);
    x=reshape(rxGrid(pssInd),[],1);
    hh=reshape(hest{2}(pssInd),[],1);
    y=hh.*pssSym;
    tmp_noise(sf/5+1)=mean(abs(x-y).^2);
end
% noiseEst(2,nreal,snr_idx)=mean(tmp_noise);

%% MMSE estimation with srsLTE 

% Compute Correlation matrices
M=40;
a=0.1;
hidx=zeros(M,1);
for i=1:M
    if (mod(i,2)==1)
        hx=floor((i-1)/2)*7+1;
    else
        hx=floor((i-1)/2)*7+5;
    end
    hidx(i)=hx;
    hp=hest{1}(hls(1,(1:P)+P*(i-1)),hx);
    Rhphp = (1-a)*Rhphp+a*(hp*hp');
    Rhhp = (1-a)*Rhhp+a*(hest{1}(:,hx)*hp');  
    Rhh = (1-a)*Rhh+a*h(:,hx)*h(:,hx)';
end
snr_lin=10^(SNR_values_db(snr_idx)/10);
Wi=((Rhphp+(1/snr_lin)*eye(P)))^-1;
W = Rhhp*Wi;

% for i=1:length(hidx)
%     hp=hls(3,(1:P)+P*(i-1));
%     hest{3}(:,hidx(i))=W*transpose(hp);
% end

w=reshape(transpose(W),1,[]);
[tmp, ~, ~, noiseEst(3,nreal,snr_idx)] = srslte_chest(enb.NCellID,enb.CellRefP,rxGrid,w);
hest{3}=reshape(tmp, size(hest{1}));


%% Robust MMSE estimation using srsLTE (Eurecom paper)
t=0:Lp-1;
alfa=log(2*Lp)/Lp;
c_l=exp(-t*alfa);
c_l=c_l/sum(c_l);
C_l=diag(1./c_l);
prows=rstart+(1:6:K);

F=dftmtx(N);
F_p=F(prows,1:Lp);
F_l=F((rstart+1):(K+rstart),1:Lp);
Wi=(F_p'*F_p+C_l/snr_lin)^(-1);
W2=F_l*Wi*F_p';

% for i=1:length(hidx)
%    hp=hls(3,(1:P)+P*(i-1));
%    hest{4}(:,hidx(i))=W2*transpose(hp);
% end

w2=reshape(transpose(W2),1,[]);
[tmp, ~, ~, noiseEst(4,nreal,snr_idx)] = srslte_chest(enb.NCellID,enb.CellRefP,rxGrid,w2);
hest{4}=reshape(tmp, size(hest{1}));



%% Another robust method from Infocom research paper
 
c_l=ones(Lp,1)/Lp;
c_l2=[c_l; zeros(N-Lp,1)];
C_l=diag(c_l2);
F=dftmtx(N);
R_hh=F*C_l*F';
R_hphp=R_hh(prows,prows);
R_hhp=R_hh((rstart+1):(K+rstart),prows);
W3=R_hhp*(R_hphp+(1/snr_lin)*eye(P))^-1;

% for i=1:length(hidx)
%     hp=hls(3,(1:P)+P*(i-1));
%     hest{5}(:,hidx(i))=W3*transpose(hp);    
% end

w3=reshape(transpose(W3),1,[]);
[tmp, ~, ~, noiseEst(5,nreal,snr_idx)] = srslte_chest(enb.NCellID,enb.CellRefP,rxGrid,w3);
hest{5}=reshape(tmp, size(hest{1}));



%% Compute MSE 
for i=1:Ntests
    MSE(i,nreal,snr_idx)=mean(mean(abs(h(:,:)-hest{i}(:,:)).^2));
    fprintf('MSE test %d: %f\n',i, 10*log10(MSE(i,nreal,snr_idx)));
end

%% Plot a single realization
if (length(SNR_values_db) == 1)
    subplot(1,2,1)
    sym=1;
    ref_idx=1:P;
    ref_idx_x=[1:6:K];% (292:6:360)-216];% 577:6:648];
    n=1:(K*length(sym));
    for i=1:Ntests
        plot(n,abs(reshape(hest{i}(:,sym),1,[])),colors2{i});
        hold on;
    end
    plot(ref_idx_x,abs(hls(3,ref_idx)),'ro');
    hold off;
    tmp=cell(Ntests+1,1);
    for i=1:Ntests
        tmp{i}=legends{i};
    end
    tmp{Ntests+1}='LS';
    legend(tmp)

    xlabel('SNR (dB)')
    ylabel('Channel Gain')
    grid on;
    
    u(1+ceil(chinfo.PathSampleDelays))=chinfo.PathGains(1,:);
    
    subplot(1,2,2)
    plot(1:P,real(W(150,:)),1:P,real(W2(150,:)),1:P,real(W3(150,:)))
    legend('mmse','robust1','robust2')
    grid on
    
    fprintf('Mean MMSE Robust %.2f dB\n', 10*log10(MSE(4,nreal,snr_idx)))
    fprintf('Mean MMSE matlab %.2f dB\n', 10*log10(MSE(1,nreal,snr_idx)))

%     u=zeros(N,1);
%     u(1+ceil(chinfo.PathSampleDelays))=mean(chinfo.PathGains(7680+(1:512+40),:));
%     
%     subplot(2,2,1)
%     plot(1:length(u),abs(u))
%     
%     subplot(2,2,2)
%     plot(abs(fftshift(fft(u,N))))
%     
%     subplot(2,2,3)
%     hf=[zeros((N-K)/2,1); h(1:K/2,1); 0; h(K/2+1:end,1); zeros((N-K)/2-1,1)];
%     plot(1:Lp,real(ifft(ifftshift(h(:,1)),Lp)))
%     
%     subplot(2,2,4)
%     plot(abs(hf))
%     
end

end
end


%% Compute average MSE and noise estimation
mean_mse=mean(MSE,2);
mean_snr=10*log10(1./mean(noiseEst,2));


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

