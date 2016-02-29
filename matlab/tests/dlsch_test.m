clear
enbConfig=struct('NCellID',0,'CyclicPrefix','Normal','CellRefP',1);
pdschConfig=struct('Modulation','64QAM','RV',1,'TxScheme','Port0','NTurboDecIts',10);

addpath('../../build/srslte/lib/phch/test')

%TBs=18336;
i=1;
e_bits=3450*6;
%error=zeros(size(TBs));
%for i=1:length(TBs)
    %trblkin=randi(2,TBs(i),1)-1;
    trblkin=read_uchar('../../build/data_in');
    
    fprintf('e_bits=%d, trblkin=%d\n',e_bits,length(trblkin));

    [mat, info]=lteDLSCH(enbConfig,pdschConfig,e_bits,trblkin);
    lib=srslte_dlsch_encode(enbConfig,pdschConfig,e_bits,trblkin);
    error(i)=mean(abs(double(mat)-double(lib)));    
    mat(mat==0)=-1;
    mat=mat*10;
    
    rec = lteRateRecoverTurbo(mat,length(trblkin),pdschConfig.RV);
    rec2 = reshape(reshape(rec{1},[],3)',[],1);
    out = lteTurboDecode(rec{1});
    
    x=read_int16('../../build/rmout_0.dat');
    subplot(2,1,1)
    plot(abs(double(x)-double(rec2)));
    t=1:100;
    %plot(t,double(x(t)),t,double(rec2(t)))
    subplot(2,1,2)
    llr=read_int16('../../build/llr.dat');
    plot(abs(double(mat)-double(llr)))
    
    [data, crc,state] = lteDLSCHDecode(enbConfig, pdschConfig, length(trblkin), mat);
    disp(crc)
%end
% 
% if (length(TBs) == 1)
%     %disp(info)
%     disp(error)
%     n=1:length(mat);
%     plot(abs(double(mat)-double(lib)))
% else
%     plot(error)
% end
