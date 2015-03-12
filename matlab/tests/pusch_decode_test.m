ueConfig=struct('NCellID',0,'NULRB',25,'NSubframe',4,'RNTI',109,'CyclicPrefixUL','Normal','NTxAnts',1);
puschConfig=struct('NLayers',1,'OrthCover','Off','PRBSet',(19:22)','Modulation','QPSK','RV',0,'Shortened',0);

subframe_rx=lteSCFDMADemodulate(ueConfig,x.*transpose(exp(-1i*2*pi*0.18*(1:length(x))/512)));
idx=ltePUSCHIndices(ueConfig,puschConfig);
pusch_rx=subframe_rx(idx);
[hest, noiseest] = lteULChannelEstimate(ueConfig,puschConfig,subframe_rx);
ce=hest(idx);
[cws,symbols] = ltePUSCHDecode(ueConfig,puschConfig,pusch_rx,ce,noiseest);
[trblkout,blkcrc,stateout] = lteULSCHDecode(ueConfig,puschConfig,88,cws);
disp(blkcrc)
scatter(real(symbols),imag(symbols))