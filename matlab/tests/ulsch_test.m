clear 
ueConfig=struct('NCellID',1,'CyclicPrefixUL','Normal','NTxAnts',1);
puschConfig=struct('NLayers',1,'OrthCover','Off','PRBSet',0,'Shortened',0);

addpath('../../build/srslte/lib/phch/test')

TBs=336;
cqilen=0;
mods={'16QAM'};
rvs=0;
betas=5;

for i=1:length(TBs)
    for m=1:length(mods)
        for r=1:length(rvs)
            for bri=1:length(betas)
                for back=1:length(betas)          
                    for c=1:length(cqilen)

                        trblkin=randi(2,TBs(i),1)-1;

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

                        if (cqilen(c)>0 || TBs(i)>0)
                            [mat, info]=lteULSCH(ueConfig,puschConfig,trblkin,ones(1,cqilen(c)),ri_bit,ack_bit,[]);
                            mat(mat==-2)=3;
                            mat(mat==-1)=2;
                            [lib]=srslte_ulsch_encode(ueConfig,puschConfig,trblkin,ones(1,cqilen(c)),ri_bit,ack_bit);
                            lib(lib==192)=3;
                            lib(lib==48)=2;
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

if (length(TBs) == 1)
    %disp(info)
    n=1:length(mat);
    %plot(abs(double(mat)-double(lib)))
    plot(n,lib(n),n,mat(n))
end
