clear

addpath('../../debug/srslte/lib/mimo/test')

Nt=1;
Nr=1;
Nl=1;
Ncw=1;
txscheme='Port0';
codebook=0;
enb.NDLRB=6;

Ns=enb.NDLRB*12*14;
enb.CyclicPrefix='Normal';
enb.CellRefP=Nt;
enb.TotSubframes=1;

cfg.Seed = 1;                  % Random channel seed
cfg.NRxAnts = Nr;               % 1 receive antenna
cfg.DelayProfile = 'ETU';      % EVA delay spread
cfg.DopplerFreq = 100;           % 120Hz Doppler frequency
cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
cfg.InitTime = 0;              % Initialize at time zero
cfg.NTerms = 16;               % Oscillators used in fading model
cfg.ModelType = 'GMEDS';       % Rayleigh fading model type
cfg.InitPhase = 'Random';      % Random initial phases
cfg.NormalizePathGains = 'On'; % Normalize delay profile power 
cfg.NormalizeTxAnts = 'On';    % Normalize for transmit antennas

cec = struct('FreqWindow',9,'TimeWindow',9,'InterpType','cubic');
cec.PilotAverage = 'UserDefined';
cec.InterpWinSize = 1;
cec.InterpWindow = 'Causal';

sym = 2*rand(Ns*Nl,1)-1;

layermap = lteLayerMap(sym, Nl, txscheme);
tx = lteDLPrecode(layermap, Nt, txscheme, codebook);

tx_srs = srslte_precoder(sym, Nl, Nt, txscheme);

err_tx=mean(abs(tx_srs-tx).^2)

[txwaveform, info] = lteOFDMModulate(enb, reshape(tx,enb.NDLRB*12,[],Nt));
cfg.SamplingRate = info.SamplingRate;

rxwaveform = lteFadingChannel(cfg, txwaveform);

rxGrid = lteOFDMDemodulate(enb, rxwaveform);
h=lteDLPerfectChannelEstimate(enb, cfg);

hp=reshape(h,Ns,Nr,Nt);    
rx=reshape(rxGrid,Ns,Nr);

if (Nt > 1) 
    if (strcmp(txscheme,'TxDiversity')==1)
        output_mat = lteTransmitDiversityDecode(rx, hp); 
    elseif (strcmp(txscheme,'CDD')==1 || strcmp(txscheme,'SpatialMux')==1)
        pdsch.NLayers=Nl;
        pdsch.RNTI=0;
        pdsch.TxScheme=txscheme;
        pdsch.PMISet=codebook;
        pdsch.NCodewords=Ncw;
        deprecoded = lteEqualizeMIMO(enb,pdsch,rx,hp,0);
        out_cw = lteLayerDemap(pdsch,deprecoded);
        output_mat = [];
        for i=1:Ncw
            output_mat = [output_mat out_cw{i}];
        end
    else
        error('Unsupported txscheme')
    end
else
    output_mat = lteEqualizeMMSE(rx, hp, 0); 
end

output_srs = srslte_predecoder(rx, hp, 0, txscheme);

plot(abs(output_mat(:)-output_srs(:)))
mean(abs(output_mat(:)-output_srs(:)).^2)

t=1:100;
plot(t,real(output_mat(t)),t,real(output_srs(t)))

