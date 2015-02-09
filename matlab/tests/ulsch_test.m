ueConfig=struct('NCellID',1,'CyclicPrefixUL','Normal','NTxAnts',1);
puschConfig=struct('NLayers',1,'OrthCover','Off','PRBSet',0,'Modulation','QPSK','RV',0,'Shortened',0);

addpath('../../debug/lte/phy/lib/phch/test')

TBs=99;
error=zeros(size(TBs));
for i=1:length(error)
    %trblkin=randi(2,TBs(i),1)-1;
    trblkin=ones(TBs(i),1);
    
    puschConfig.BetaCQI = 7.0; 
    puschConfig.BetaRI = 11.0;
    puschConfig.BetaACK = 5.0;

    [mat, info]=lteULSCH(ueConfig,puschConfig,trblkin,[ones(1,20)],[1],[0],[]);
    mat(mat==-2)=2;
    mat(mat==-1)=3;
    [lib]=liblte_ulsch_encode(ueConfig,puschConfig,trblkin,[ones(1,20)],[1],[0]);
    error(i)=sum(abs(double(mat)-double(lib)));
    if (length(TBs) == 1)
        disp(error(i))    
    end
end

if (length(TBs) == 1)
    %disp(info)
    n=1:length(mat);
    plot(abs(double(mat)-double(lib)))
else
    plot(error)
    disp(sum(error))
end
