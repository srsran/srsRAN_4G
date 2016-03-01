clear

enbConfig=struct('NCellID',1,'CyclicPrefix','Normal','CellRefP',1);
pdschConfig=struct('Modulation','QPSK','RV',0,'TxScheme','Port0');

addpath('../../build/srslte/lib/fec/test')


codewordLen = 12000;
trBlkLenVec = 6201;
error=zeros(size(trBlkLenVec));
for i=1:length(trBlkLenVec)
    trBlkLen=trBlkLenVec(i);
    in=randi(2,trBlkLen,1)-1;
   
    RV = 0;
    trblockwithcrc = lteCRCEncode(in,'24A');
    codeblocks = lteCodeBlockSegment(trblockwithcrc);
    turbocodedblocks = lteTurboEncode(codeblocks);    
    codeword = lteRateMatchTurbo(turbocodedblocks,codewordLen,RV);
    
    info = lteDLSCHInfo(trBlkLen); 
    
    mat = lteRateRecoverTurbo(codeword,trBlkLen,RV);
    mat2 = reshape(reshape(mat{1},[],3)',[],1);
    out = lteTurboDecode(mat{1});
    
    lib=srslte_rm_turbo_rx(double(codeword),trBlkLen,RV);
    [outLib] = srslte_turbodecoder(lib);
    
    if (length(trBlkLenVec) == 1)
%        fprintf('Err mat: %d. Err lib: %d\n',sum(out(1+info.F:trBlkLen+info.F)~=in),sum(outLib(1+info.F:trBlkLen+info.F)~=in));
    end
    error(i)=mean(abs(double(mat2)-double(lib)));
end

if (length(trBlkLenVec) == 1)
    n=1:length(mat{1});
    plot(abs(double(mat2)-double(lib)))
    %plot(n,mat{1},n,lib)
else
    plot(error)
end
