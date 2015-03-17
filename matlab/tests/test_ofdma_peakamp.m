clear 
enbConfig=struct('NCellID',0,'DuplexMode','TDD','CyclicPrefix','Normal','CellRefP',1,'CFI',2,'NDLRB',25,'NSubframe',4,'CyclicPrefixUL','Normal','NTxAnts',1);
pdschConfig=struct('NLayers',1,'TxScheme','Port0','OrthCover','Off','PRBSet',0,'RNTI',82,'Modulation','QPSK','RV',0,'Shortened',0);

addpath('../../debug/lte/phy/lib/phch/test')

NDLRB=[6 15 25 50 100];

Peak=[];
k=1;
for r=1:length(NDLRB)
    fprintf('NDLRB: %d\n',NDLRB(r));
    for l=1:NDLRB(r)                
        trblkin=randi(2,l*5,1)-1;

        enbConfig.NDLRB=NDLRB(r);
        pdschConfig.PRBSet=(0:(l-1))';

        idx=ltePDSCHIndices(enbConfig,pdschConfig,pdschConfig.PRBSet);
        [cw, info]=lteDLSCH(enbConfig,pdschConfig,2*length(idx),trblkin);
        cw_mat=ltePDSCH(enbConfig,pdschConfig,cw);
        subframe_mat = lteDLResourceGrid(enbConfig);
        subframe_mat(idx)=cw_mat;
        waveform = lteOFDMModulate(enbConfig,subframe_mat,0);
        waveform = waveform*sqrt(512)/sqrt(l)*NDLRB(r)/15;
        Peak(k)=max(max(abs(real(waveform))),max(abs(imag(waveform))));
        k=k+1;
    end
end

plot(Peak(:)')

