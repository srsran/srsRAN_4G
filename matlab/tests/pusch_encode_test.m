ueConfig=struct('NCellID',1,'NULRB',6,'CyclicPrefixUL','Normal','NTxAnts',1,'RNTI',64);
puschConfig=struct('NLayers',1,'OrthCover','Off','Shortened',0,'NBundled',0);

addpath('../../build//srslte/lib/phch/test')

cqilen=[0 4 20];
mods={'64QAM'};
rvs=0;
betas=[0 5.0, 20.0];
for p=1:ueConfig.NULRB
    for i=0:26
        for m=1:length(mods)
            for r=1:length(rvs)
                for bri=1:length(betas)
                    for back=1:length(betas)          
                        for c=1:length(cqilen)
                            for s=0:9
                                ueConfig.NSubframe=s;
                                puschConfig.PRBSet=(0:p-1)';

                                TBs=lteTBS(length(puschConfig.PRBSet),i);
                                %TBs=256;
                                %trblkin=randi(2,TBs,1)-1;
                                trblkin=ones(1,TBs);

                                puschConfig.Modulation = mods{m};
                                puschConfig.RV = rvs(r);
                                puschConfig.BetaCQI = 5; 
                                puschConfig.BetaRI = betas(bri);
                                puschConfig.BetaACK = betas(back);

                               if (betas(bri)>0)
                                   ri_bit=randi(2,1,1)-1;
                               else
                                   ri_bit=[];
                               end
                               if (betas(back)>0)
                                   ack_bit=randi(2,1,1)-1;
                               else
                                   ack_bit=[];
                               end

                                if (cqilen(c)>0 || TBs>0)
                                    [enc, info]=lteULSCH(ueConfig,puschConfig,trblkin,ones(1,cqilen(c)),ri_bit,ack_bit);
                                    cw_mat=ltePUSCH(ueConfig,puschConfig,enc);
                                    %[drs, infodrs]=ltePUSCHDRS(ueConfig,puschConfig);
                                    idx=ltePUSCHIndices(ueConfig,puschConfig);
                                    %drs_idx=ltePUSCHDRSIndices(ueConfig,puschConfig);
                                    subframe_mat = lteULResourceGrid(ueConfig);
                                    subframe_mat(idx)=cw_mat;

                                    [~, subframe_lib, cwlib, bits]=srslte_pusch_encode(ueConfig,puschConfig,trblkin,ones(1,cqilen(c)),ri_bit,ack_bit);
                                    err=max(abs(subframe_mat(:)-subframe_lib));
                                    if (err > 10^-5)
                                      plot(abs(subframe_mat(:)-subframe_lib))
                                      error('Error!');
                                    end
                                end
                            end
                        end
                    end
                end
            end
        end
    end
end

