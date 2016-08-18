ueConfig=struct('NCellID',1,'NULRB',15,'NSubframe',0,'RNTI',30,'CyclicPrefixUL','Normal','NTxAnts',1,'Shortened',0);
puschConfig=struct('NTurboDecIts',5,'NLayers',1,'OrthCover','Off','PRBSet',(0:14)','Modulation','QPSK','RV',0);

TBS=392;
cfo=0;
t0=1;
x=[rx(t0:end); zeros(t0-1,1)];

x=lteFrequencyCorrect(ueConfig,x,cfo);
subframe_rx=lteSCFDMADemodulate(ueConfig,x);
idx=ltePUSCHIndices(ueConfig,puschConfig);
pusch_rx=subframe_rx(idx);
dmrs_rx=subframe_rx(ltePUSCHDRSIndices(ueConfig,puschConfig));
[hest, noiseest] = lteULChannelEstimate(ueConfig,puschConfig,subframe_rx);
ce=hest(idx);
[cws,symbols] = ltePUSCHDecode(ueConfig,puschConfig,pusch_rx,ce,noiseest);
[trblkout,blkcrc,stateout] = lteULSCHDecode(ueConfig,puschConfig,TBS,cws);
disp(blkcrc)
scatter(real(symbols),imag(symbols))
%plot(angle(hest(:,1)))