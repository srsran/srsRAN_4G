clear

addpath('../../debug/srslte/lib/mimo/test')

%enb = lteRMCDL('R.10'); % 2-ports
enb = lteRMCDL('R.0'); % 1-ports

cec = struct('FreqWindow',9,'TimeWindow',9,'InterpType','cubic');
cec.PilotAverage = 'UserDefined';
cec.InterpWinSize = 1;
cec.InterpWindow = 'Causal';

cfg.Seed = 1;                  % Random channel seed
cfg.NRxAnts = 1;               % 1 receive antenna
cfg.DelayProfile = 'ETU';      % EVA delay spread
cfg.DopplerFreq = 100;           % 120Hz Doppler frequency
cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
cfg.InitTime = 0;              % Initialize at time zero
cfg.NTerms = 16;               % Oscillators used in fading model
cfg.ModelType = 'GMEDS';       % Rayleigh fading model type
cfg.InitPhase = 'Random';      % Random initial phases
cfg.NormalizePathGains = 'On'; % Normalize delay profile power 
cfg.NormalizeTxAnts = 'On';    % Normalize for transmit antennas

[txWaveform, ~, info] = lteRMCDLTool(enb,[1;0;0;1]);
cfg.SamplingRate = info.SamplingRate;

txWaveform = txWaveform+complex(randn(size(txWaveform)),randn(size(txWaveform)))*1e-3;

rxWaveform = lteFadingChannel(cfg,txWaveform);

rxGrid = lteOFDMDemodulate(enb,rxWaveform);

[h,n0] = lteDLChannelEstimate(enb,cec,rxGrid);

s=size(h);
p=s(1);
n=s(2);
if (length(s)>2)
    Nr=s(3);
else
    Nr=1;
end
if (length(s)>3)
    Nt=s(4);
else
    Nt=1;
end

if (Nr > 1)
    rx=reshape(rxGrid,p,n,Nr);
    hp=reshape(h,p,n,Nr,Nt);    
else
    rx=rxGrid;
    hp=h;
end

if (Nt > 1) 
    output_mat = lteTransmitDiversityDecode(rx, hp); 
else
    output_mat = lteEqualizeMMSE(rx, hp, n0); 
end
output_srs = srslte_diversitydecode(rx, hp, n0);

plot(abs(output_mat(:)-output_srs(:)))
mean(abs(output_mat(:)-output_srs(:)).^2)

t=1:10;
plot(t,real(output_mat(t)),t,real(output_srs(t)))

