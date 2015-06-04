ueConfig=struct('NCellID',1,'NULRB',25,'NSubframe',8,'RNTI',71,'CyclicPrefixUL','Normal','NTxAnts',1,'Shortened',0);
puschConfig=struct('NTurboDecIts',5,'NLayers',1,'OrthCover','Off','PRBSet',22,'Modulation','16QAM','RV',0);

TBS=336;
cfo=3400;
t0=128;
x=[rx(t0:end); zeros(t0-1,1)];

subframe_rx=lteSCFDMADemodulate(ueConfig,x.*exp(-1i*2*pi*cfo/15000*transpose(1:length(x))/512));
idx=ltePUSCHIndices(ueConfig,puschConfig);
pusch_rx=subframe_rx(idx);
[hest, noiseest] = lteULChannelEstimate(ueConfig,puschConfig,subframe_rx);
ce=hest(idx);
[cws,symbols] = ltePUSCHDecode(ueConfig,puschConfig,pusch_rx,ce,noiseest);
[trblkout,blkcrc,stateout] = lteULSCHDecode(ueConfig,puschConfig,TBS,cws);
disp(blkcrc)
scatter(real(symbols),imag(symbols))
%plot(angle(hest(:,1)))