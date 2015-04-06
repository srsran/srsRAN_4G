ueConfig=struct('CyclicPrefixUL','Normal','NTxAnts',1,'NULRB',6);
puschConfig=struct('NLayers',1,'OrthCover','Off');

addpath('../../debug/lte/phy/lib/ch_estimation/test')

Hopping={'Off','Sequence','Group'};

k=1;
for prb=4
    for ncell=1
        for ns=8
            for h=1
                for sg=0
                    for cs=0
                       for ds=0
                           
                            ueConfig.NCellID=ncell;
                            ueConfig.NSubframe=ns;
                            ueConfig.Hopping=Hopping{h};
                            ueConfig.SeqGroup=sg;
                            ueConfig.CyclicShift=cs;

                            puschConfig.PRBSet=(1:4)';
                            puschConfig.DynCyclicShift=ds;

                            [mat, info]=ltePUSCHDRS(ueConfig,puschConfig);
                            ind=ltePUSCHDRSIndices(ueConfig, puschConfig);
                            subframe_mat = lteULResourceGrid(ueConfig);
                            subframe_mat(ind)=mat;
                            
                            subframe_lib=srslte_refsignal_pusch(ueConfig,puschConfig);

                            error(k)=mean(abs(subframe_mat(:)-subframe_lib(:)));
                            disp(error(k))
                            if (error(k) > 10^-3)
                                k=1;
                            end
                            k=k+1;
                       end
                    end
                end
            end
        end
    end
end
    
plot(error);
disp(info)
disp(length(subframe_mat))
n=1:length(subframe_mat(:));
%plot(n,real(subframe_mat(:)),n,real(subframe_lib(:)))
plot(abs(subframe_mat(:)-subframe_lib(:)))
