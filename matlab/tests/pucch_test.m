clear 
ueConfig=struct('NCellID',1,'RNTI',11,'NULRB',25,'NSubframe',0,'CyclicPrefixUL','Normal','NTxAnts',1,'Hopping','Off');
pucchConfig=struct('NLayers',1,'OrthCover','Off','Shortened',0,'ResourceSize',2);

addpath('../../build/srslte/lib/phch/test')

format_str={'1','1a','1b','2','2a','2b'};

k=1;
for f=0:5
    for n=0:7:130
        for d=1:3
            for ncs=0:d:7
                for nsf=0:9
                    pucchConfig.ResourceIdx= n;
                    pucchConfig.DeltaShift = d;
                    pucchConfig.CyclicShifts = ncs;
                    ueConfig.NSubframe=nsf;
                    if (f >= 3) 
                        nb=20;
                        nb_ack=f-3;
                    else 
                        nb=f;
                        nb_ack=0;
                    end
                    bits=mod(0:nb-1,2);
                    bits_ack=randi(2,nb_ack,1)-1;
                    fprintf('Testing PUCCH Format: %s, n_pucch=%d, DeltaShift=%d, CyclicShift=%d, Nsf=%d\n',format_str{f+1},n,d,ncs,nsf);
                    if (f >= 3)
                        [sym_mat, info]=ltePUCCH2(ueConfig,pucchConfig,bits);
                        idx=ltePUCCH2Indices(ueConfig,pucchConfig);
                        [dmrs_mat, info_dmrs]=ltePUCCH2DRS(ueConfig,pucchConfig,bits_ack);
                        idx_dmrs=ltePUCCH2DRSIndices(ueConfig,pucchConfig);                                        
                    else
                        [sym_mat, info]=ltePUCCH1(ueConfig,pucchConfig,bits);
                        idx=ltePUCCH1Indices(ueConfig,pucchConfig);
                        [dmrs_mat, info_dmrs]=ltePUCCH1DRS(ueConfig,pucchConfig);
                        idx_dmrs=ltePUCCH1DRSIndices(ueConfig,pucchConfig);                                        
                    end
                    subframe_mat = lteULResourceGrid(ueConfig);
                    subframe_mat(idx)=sym_mat;
                    subframe_mat(idx_dmrs)=dmrs_mat;

                    [sym, dmrs, subframe]=srslte_pucch_encode(ueConfig,pucchConfig,[bits; bits_ack]);

                    error_sym=max(abs(sym-sym_mat));
                    error_dmrs=max(abs(dmrs-dmrs_mat));
                    error_sf=max(abs(subframe_mat(:)-subframe));
                    k=k+1;

                    if (error_sym > 1e-5)
                        disp(info)
                        plot(1:length(sym),sym,1:length(sym_mat),sym_mat)
                        legend('srsLTE','Matlab')
                        error('Error in symbols');                
                    end
                    if (error_dmrs > 1e-5)
                        disp(info_dmrs)
                        plot(angle(dmrs)-angle(dmrs_mat))
                        error('Error in DMRS');                
                    end
                    if (error_sf > 1e-5)
                        disp(info)
                        plot(abs(subframe-subframe_mat(:)))
                        error('Error in subframe');
                    end
                end
            end
        end
    end
end

if (k == 2) 
    disp(info)
    disp(error_sym)
    disp(error_sf)
    n=1:length(sym);
    plot(n,real(sym(n)),n,real(sym_mat(n)))
end
