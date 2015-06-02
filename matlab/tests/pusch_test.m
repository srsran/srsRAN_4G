ueConfig=struct('NCellID',1,'NULRB',25,'NSubframe',8,'RNTI',77,'CyclicPrefixUL','Normal','NTxAnts',1);
puschConfig=struct('NLayers',1,'OrthCover','Off','PRBSet',24,'Modulation','QAM16','RV',0,'Shortened',0);

addpath('../../debug/srslte/lib/phch/test')

 TBs=336;
 cqilen=0;
 rvs=0;
 mods={'16QAM'};
 betas=0;
 subf=8;
 
 trblkin=[0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0];
 
for i=1:length(TBs)
    for m=1:length(mods)
        for r=1:length(rvs)
            for bcqi=1:length(betas)
                for bri=1:length(betas)
                    for back=1:length(betas)          
                        for c=1:length(cqilen)
                            for s=1:length(subf)
                                fprintf('Subf=%d, RV=%d\n', subf(s), rvs(r));
                                %trblkin=randi(2,TBs(i),1)-1;
                                ueConfig.NSubframe=subf(s);
                                puschConfig.Modulation = mods{m};
                                puschConfig.RV = rvs(r);
                                puschConfig.BetaCQI = betas(bcqi); 
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

                                if (cqilen(c)>0 || TBs(i)>0)
                                    [cw, info]=lteULSCH(ueConfig,puschConfig,trblkin);
                                    cw_mat=ltePUSCH(ueConfig,puschConfig,cw);
                                    drs=ltePUSCHDRS(ueConfig,puschConfig);
                                    idx=ltePUSCHIndices(ueConfig,puschConfig);
                                    drs_ind = ltePUSCHDRSIndices(ueConfig,puschConfig);
                                    subframe_mat = lteULResourceGrid(ueConfig);
                                    subframe_mat(idx)=cw_mat;
                                    subframe_mat(drs_ind)=drs;
                                    waveform = lteSCFDMAModulate(ueConfig,subframe_mat,0);
                                    plot(abs(x-waveform*sqrt(512)))
                                    
                                    
%                                     [waveform_lib, subframe_lib, cwlib]=srslte_pusch_encode(ueConfig,puschConfig,trblkin,ones(1,cqilen(c)),ri_bit,ack_bit);
%                                     err=mean(abs(waveform-waveform_lib));
%                                     if (err > 10^-6)
%                                       disp(err)    
%                                       error('Error!');
%                                     end
                                end
                            end
                        end
                    end
                end
            end
        end
    end
end

