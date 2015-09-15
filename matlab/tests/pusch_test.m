ueConfig=struct('NCellID',1,'NULRB',6,'RNTI',64,'CyclicPrefixUL','Normal','NTxAnts',1,'Shortened',0,'Hopping','Off','SeqGroup',0,'CyclicShift',0);
puschConfig=struct('NLayers',1,'OrthCover','Off','PRBSet',0,'Shortened',0,'DynCyclicShift',0);

addpath('../../build/srslte/lib/phch/test')

TBs=336;
cqilen=0;
rvs=0;
mods={'16QAM'};
betas=5;
subf=0;

for i=1:length(TBs)
    for m=1:length(mods)
        for r=1:length(rvs)
            for bcqi=1:length(betas)
                for bri=1:length(betas)
                    for back=1:length(betas)          
                        for c=1:length(cqilen)
                            for s=1:length(subf)
                                fprintf('Subf=%d, RV=%d\n', subf(s), rvs(r));
                                trblkin=mod(0:TBs(i)-1,2);
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
                                    [enc, info]=lteULSCH(ueConfig,puschConfig,trblkin,ones(1,cqilen(c)),ri_bit,ack_bit);
                                    cw_mat=ltePUSCH(ueConfig,puschConfig,enc);
                                    %[drs, infodrs]=ltePUSCHDRS(ueConfig,puschConfig);
                                    idx=ltePUSCHIndices(ueConfig,puschConfig);
                                    %drs_idx=ltePUSCHDRSIndices(ueConfig,puschConfig);
                                    subframe_mat = lteULResourceGrid(ueConfig);
                                    subframe_mat(idx)=cw_mat;
                                    %subframe_mat(drs_idx)=drs;
                                    waveform = lteSCFDMAModulate(ueConfig,subframe_mat,0);
                                     
                                    cw_scram=lteULScramble(enc,0,1,64);
%                                 
                                    [waveform_lib, subframe_lib, cwlib, bits]=srslte_pusch_encode(ueConfig,puschConfig,trblkin,ones(1,cqilen(c)),ri_bit,ack_bit);
                                    err=max(abs(waveform-waveform_lib));
                                    if (err > 10^-5)
                                      disp(err)    
                                      t=1:200;
                                      %plot(t,bits(t),t,cw_scram(t))
                                      plot(abs(double(bits)-double(cw_scram)))
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

