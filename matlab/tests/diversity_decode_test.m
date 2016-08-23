clear

addpath('../../build/srslte/lib/mimo/test')

enb = lteRMCDL('R.10');

cec = struct('FreqWindow',9,'TimeWindow',9,'InterpType','cubic');
cec.PilotAverage = 'UserDefined';
cec.InterpWinSize = 1;
cec.InterpWindow = 'Causal';

cfg.Seed = 1;                  % Random channel seed
cfg.NRxAnts = 2;               % 1 receive antenna
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
n = length(txWaveform);
cfg.SamplingRate = info.SamplingRate;

txWaveform = txWaveform+complex(randn(n,2),randn(n,2))*1e-3;

rxWaveform = lteFadingChannel(cfg,txWaveform);

rxGrid = lteOFDMDemodulate(enb,rxWaveform);

[h,n0] = lteDLChannelEstimate(enb,cec,rxGrid);

s=size(h);
p=s(1);
Nt=s(4);
Nr=s(3);

rx=reshape(rxGrid(:,1,:),p,Nr);
hp=reshape(h(:,1,:,:),p,Nr,Nt);

output_mat = lteTransmitDiversityDecode(rx, hp); 
output_srs = srslte_diversitydecode(rx, hp);

plot(abs(output_mat-output_srs))
mean(abs(output_mat-output_srs).^2)


