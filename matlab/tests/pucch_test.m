clear 
ueConfig=struct('NCellID',2,'NULRB',6,'NSubframe',7,'CyclicPrefixUL','Normal','NTxAnts',1,'Hopping','Off');
pucchConfig=struct('NLayers',1,'OrthCover','Off','Shortened',0,'ResourceSize',0);

addpath('../../debug/srslte/lib/phch/test')

k=1;
for f=0:2
    for n=0:130
        for d=1:3
            for ncs=0:d:7
                pucchConfig.ResourceIdx= n;
                pucchConfig.DeltaShift = d;
                pucchConfig.CyclicShifts = ncs;
                ack=randi(2,f,1)-1;
                fprintf('Testint Format: %d, n_pucch=%d, DeltaShift=%d, CyclicShift=%d\n',f,n,d,ncs);
                [sym_mat, info]=ltePUCCH1(ueConfig,pucchConfig,ack);
                idx=ltePUCCH1Indices(ueConfig,pucchConfig);
                [dmrs_mat, info_dmrs]=ltePUCCH1DRS(ueConfig,pucchConfig);
                idx_dmrs=ltePUCCH1DRSIndices(ueConfig,pucchConfig);
                subframe_mat = lteULResourceGrid(ueConfig);
                subframe_mat(idx)=sym_mat;
                subframe_mat(idx_dmrs)=dmrs_mat;

                [sym, dmrs]=srslte_pucch_encode(ueConfig,pucchConfig,ack);

                error_sym=mean(abs(sym-sym_mat));
                error_dmrs=mean(abs(dmrs-dmrs_mat));
                %error_sf=mean(abs(subframe_mat(:)-subframe_lib));
                k=k+1;

                if (error_sym > 1e-6)
                    disp(info)
                    plot(angle(sym)-angle(sym_mat))
                    error('Error in symbols');                
                end
                if (error_dmrs > 1e-6)
                    disp(info_dmrs)
                    plot(angle(dmrs)-angle(dmrs_mat))
                    error('Error in DMRS');                
                end
%                 if (error_sf > 1e-6)
%                     disp(info)
%                     p=1:length(subframe_lib);
%                     plot(p,real(subframe_lib(p)),p,real(subframe_mat(p)))
%                     error('Error in subframe');
%                 end
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
