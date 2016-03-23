clear

% Run pdsch_test with -vv to generate files. Then run this script to check
% rate matching and demodulation
% Need to change soft demodulator output to +-10

enbConfig=struct('NCellID',0,'CyclicPrefix','Normal','CellRefP',1,'DuplexMode','FDD');
pdschConfig=struct('Modulation','64QAM','RV',3,'TxScheme','Port0','NTurboDecIts',10,...
                   'NSoftbits',0,'DuplexMode','FDD');

addpath('../../build/srslte/lib/phch/test')

cbidx_v=0:12;
e_bits=90000;

trblkin=read_uchar('../../build/data_in');
[mat, info]=lteDLSCH(enbConfig,pdschConfig,e_bits,trblkin);
mat(mat==0)=-1;
mat=mat*10;
rec = lteRateRecoverTurbo(mat,length(trblkin),pdschConfig.RV,pdschConfig);

rec2=cell(size(rec));
srs=cell(size(rec));
for cbidx=cbidx_v
    rec2{cbidx+1} = reshape(reshape(rec{cbidx+1},[],3)',[],1);
    srs{cbidx+1}=read_int16(sprintf('../../build/rmout_%d.dat',cbidx));
end


subplot(2,1,1)
plot(abs(double(reshape(cell2mat(srs),1,[]))-double(reshape(cell2mat(rec2),1,[]))));

subplot(2,1,2)
llr=read_int16('../../build/llr.dat');
plot(abs(double(mat)-double(llr)))

[data, crc,state] = lteDLSCHDecode(enbConfig, pdschConfig, length(trblkin), mat);
disp(crc)

