ueConfig=struct('NCellID',1,'NULRB',25,'NSubframe',0,'RNTI',65,'CyclicPrefixUL','Normal','NTxAnts',1);
puschConfig=struct('NLayers',1,'OrthCover','Off','PRBSet',24,'Modulation','QPSK','RV',0,'Shortened',0);

TBS=72;
cfo=1146;

subframe_rx=lteSCFDMADemodulate(ueConfig,x.*exp(-1i*2*pi*cfo/15000*transpose(1:length(x))/512));
idx=ltePUSCHIndices(ueConfig,puschConfig);
pusch_rx=subframe_rx(idx);
[hest, noiseest] = lteULChannelEstimate(ueConfig,puschConfig,subframe_rx);
ce=hest(idx);
[cws,symbols] = ltePUSCHDecode(ueConfig,puschConfig,pusch_rx,ce,noiseest);
[trblkout,blkcrc,stateout] = lteULSCHDecode(ueConfig,puschConfig,TBS,cws);
disp(blkcrc)
scatter(real(symbols),imag(symbols))
