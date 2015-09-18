clear 
ueConfig=struct('NCellID',1,'CyclicPrefixUL','Normal','NTxAnts',1);
puschConfig=struct('NLayers',1,'OrthCover','Off','Shortened',0);

addpath('/home/ismael/work/srsLTE/debug/srslte/lib/phch/test')

cqilen=[0 20];
mods={'QPSK','16QAM', '64QAM'};
rvs=0;
betas=[0 2.0 2.5 5.0, 20.0];
for p=1:100
    for i=0:26
        for m=1:length(mods)
            for r=1:length(rvs)
                for bri=1:length(betas)
                    for back=1:length(betas)          
                        for c=1:length(cqilen)
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
                                [lib]=srslte_ulsch_encode(ueConfig,puschConfig,trblkin,ones(1,cqilen(c)),ri_bit,ack_bit);
                                [mat, info]=lteULSCH(ueConfig,puschConfig,trblkin,ones(1,cqilen(c)),ri_bit,ack_bit,[]);
                                mat(mat==-2)=0;
                                mat(mat==-1)=0;
                                err=sum(abs(double(mat)-double(lib)));
                                if (err > 0)
                                  disp(err)    
                                  plot(abs(double(mat)-double(lib)))
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

if (length(TBs) == 1)
    %disp(info)
    n=1:length(mat);
    %plot(abs(double(mat)-double(lib)))
    plot(n,lib(n),n,mat(n))
end
