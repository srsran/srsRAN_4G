ueConfig=struct('CyclicPrefixUL','Normal','NTxAnts',1);
puschConfig=struct('NLayers',1,'OrthCover','Off');

addpath('../../debug/lte/phy/lib/ch_estimation/test')

Hopping={'Off','Sequence','Group'};

k=1;
for prb=6:6
    for ncell=0:2
        for ns=0:9
            for h=1:3
                for sg=0:29
                    for cs=0:7
                       for ds=0:7
                           
                            ueConfig.NCellID=ncell;
                            ueConfig.NSubframe=ns;
                            ueConfig.Hopping=Hopping{h};
                            ueConfig.SeqGroup=sg;
                            ueConfig.CyclicShift=cs;

                            puschConfig.PRBSet=(0:(prb-1))';
                            puschConfig.DynCyclicShift=ds;

                            [mat, info]=ltePUSCHDRS(ueConfig,puschConfig);
                            lib=liblte_refsignal_pusch(ueConfig,puschConfig);

                            error(k)=mean(abs(mat-lib));
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
disp(length(mat))
n=1:length(mat);
plot(n,real(mat(n)),n,real(lib(n)))

