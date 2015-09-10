enbConfig=struct('NCellID',1,'CyclicPrefix','Normal','CellRefP',1);
pdschConfig=struct('Modulation','QPSK','RV',0,'TxScheme','Port0');

addpath('/home/ismael/work/srsLTE/debug/srslte/lib/phch/test')

TBs=40:8:800;
e_bits=1000;
error=zeros(size(TBs));
for i=1:length(TBs)
    trblkin=randi(2,TBs(i),1)-1;
    %trblkin=ones(104,1);
    %trblkin=[1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, ];
    %trblkin=[1, 0];
    
    [mat, info]=lteDLSCH(enbConfig,pdschConfig,e_bits,trblkin);
    lib=srslte_dlsch_encode(enbConfig,pdschConfig,e_bits,trblkin);
    error(i)=mean(abs(double(mat)-double(lib)));    
end

if (length(TBs) == 1)
    %disp(info)
    disp(error)
    n=1:length(mat);
    plot(abs(double(mat)-double(lib)))
else
    plot(error)
end
