clear 
ueConfig=struct('NCellID',1,'NULRB',100,'DuplexMode','FDD','NSubframe',0,'CyclicPrefixUL','Normal','NTxAnts',1);
srsConfig=struct('NTxAnts',1,'ConfigIdx',317,'SeqGroup',1,'SeqIdx',0,'TxComb',0);

addpath('../../build/srslte/lib/ch_estimation/test')

for k=0:50
    for csbw=0:7
        for uebw=0:3
            for hop=0:3
                for ncs=0            
                    for n_rrc=1:5:20
                        ueConfig.NFrame=mod(32*k,1024);
                        srsConfig.BWConfig = csbw;
                        srsConfig.BW = uebw;
                        srsConfig.CyclicShift = ncs; 
                        srsConfig.HoppingBW = hop;
                        srsConfig.FreqPosition = n_rrc;
                        fprintf('Testing SRS: Nframe=%d, CSBW=%d, UEBW=%d, b_hop=%d n_rrc=%d\n',ueConfig.NFrame, csbw, uebw, hop, n_rrc);

                        [sym_mat, info]=lteSRS(ueConfig,srsConfig);
                        [idx, info2]=lteSRSIndices(ueConfig,srsConfig);
                        subframe_mat = lteULResourceGrid(ueConfig);
                        subframe_mat(idx)=sym_mat;

                        [sym, subframe]=srslte_refsignal_srs(ueConfig,srsConfig);

                        error_sym=max(abs(sym-sym_mat));
                        error_sf=max(abs(subframe_mat(:)-subframe));

                        if (error_sym > 3.5e-3)
                            disp(info)
                            plot(abs(sym-sym_mat))
                            legend('srsLTE','Matlab')
                            error('Error in symbols');                
                        end
                        if (error_sf > 3.5e-3)
                            disp(info2)
                            plot(abs(subframe-subframe_mat(:)))
                            error('Error in subframe');
                        end
                    end
                end
            end
        end
    end
end