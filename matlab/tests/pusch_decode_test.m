ueConfig=struct('NCellID',1,'NULRB',6,'NSubframe',8,'RNTI',81,'CyclicPrefixUL','Normal','NTxAnts',1);
puschConfig=struct('NLayers',1,'OrthCover','Off','PRBSet',(1:4)','Modulation','QPSK','RV',0,'Shortened',0);

subframe_rx=lteSCFDMADemodulate(ueConfig,x.*transpose(exp(-1i*2*pi*0.26*(1:length(x))/128)));
idx=ltePUSCHIndices(ueConfig,puschConfig);
pusch_rx=subframe_rx(idx);
[hest, noiseest] = lteULChannelEstimate(ueConfig,puschConfig,subframe_rx);
ce=hest(idx);
[cws,symbols] = ltePUSCHDecode(ueConfig,puschConfig,pusch_rx,ce,noiseest);
[trblkout,blkcrc,stateout] = lteULSCHDecode(ueConfig,puschConfig,88,cws);
disp(blkcrc)
subplot(1,2,1)
scatter(real(symbols),imag(symbols))
subplot(1,2,2)
plot(angle(hest))
