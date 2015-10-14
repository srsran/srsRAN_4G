clear
enbConfig=struct('NCellID',1,'CyclicPrefix','Normal','CellRefP',1);
pdschConfig=struct('Modulation','64QAM','RV',0,'TxScheme','Port0');

addpath('../../build/srslte/lib/phch/test')

TBs=36696;
e_bits=41400;
error=zeros(size(TBs));
for i=1:length(TBs)
    trblkin=randi(2,TBs(i),1)-1;
    
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
