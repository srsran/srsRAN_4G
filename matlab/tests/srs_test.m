clear 
ueConfig=struct('NCellID',1,'NULRB',25,'DuplexMode','FDD','NSubframe',0,'NFrame',0,'CyclicPrefixUL','Normal','NTxAnts',1);
srsConfig=struct('NTxAnts',1,'ConfigIdx',0,'SeqGroup',1,'SeqIdx',0,'TxComb',0);

addpath('../../build/srslte/lib/ch_estimation/test')

for csbw=2:7
    for uebw=0:3
        for hop=0:3
            for ncs=0:7            
                for n_rrc=1:23
                    srsConfig.BWConfig = csbw;
                    srsConfig.BW = uebw;
                    srsConfig.CyclicShift = ncs; 
                    srsConfig.HoppingBW = hop;
                    srsConfig.FreqPosition = n_rrc;
                    fprintf('Testing SRS: CSBW=%d, UEBW=%d, b_hop=%d n_rrc=%d, CyclicShift=%d\n',csbw, uebw, hop, n_rrc, ncs);

                    [sym_mat, info]=lteSRS(ueConfig,srsConfig);
                    [idx, info2]=lteSRSIndices(ueConfig,srsConfig);
                    subframe_mat = lteULResourceGrid(ueConfig);
                    subframe_mat(idx)=sym_mat;

                    [sym, subframe]=srslte_refsignal_srs(ueConfig,srsConfig);

                    error_sym=max(abs(sym-sym_mat));
                    error_sf=max(abs(subframe_mat(:)-subframe));

                    if (error_sym > 1e-3)
                        disp(info)
                        plot(1:length(sym),sym,1:length(sym_mat),sym_mat)
                        legend('srsLTE','Matlab')
                        error('Error in symbols');                
                    end
                    if (error_sf > 1e-3)
                        disp(info2)
                        plot(abs(subframe-subframe_mat(:)))
                        error('Error in subframe');
                    end
                end
            end
        end
    end
end
