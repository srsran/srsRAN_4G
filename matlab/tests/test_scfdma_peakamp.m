clear 
ueConfig=struct('NCellID',0,'NULRB',25,'NSubframe',4,'RNTI',82,'CyclicPrefixUL','Normal','NTxAnts',1);
puschConfig=struct('NLayers',1,'OrthCover','Off','PRBSet',0,'Modulation','QPSK','RV',0,'Shortened',0);

addpath('../../debug/lte/phy/lib/phch/test')

NULRB=[6 15 25 50 100];

Peak=[];
k=1;
for r=1:length(NULRB)
    fprintf('NULRB: %d\n',NULRB(r));
    for l=1:NULRB(r)                
        trblkin=randi(2,l*5,1)-1;

        ueConfig.NULRB=NULRB(r);
        puschConfig.PRBSet=(0:(l-1))';

        [cw, info]=lteULSCH(ueConfig,puschConfig,trblkin);
        cw_mat=ltePUSCH(ueConfig,puschConfig,cw);
        idx=ltePUSCHIndices(ueConfig,puschConfig);
        subframe_mat = lteULResourceGrid(ueConfig);
        subframe_mat(idx)=cw_mat;
        waveform = lteSCFDMAModulate(ueConfig,subframe_mat,0);
        waveform = waveform*sqrt(512)/sqrt(l)*NULRB(r)/10;
        Peak(k)=max(max(abs(real(waveform))),max(abs(imag(waveform))));
        k=k+1;
    end
end

plot(Peak(:)')

