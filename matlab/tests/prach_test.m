clear 
ueConfig=struct('NULRB',6,'DuplexMode','FDD','CyclicPrefix','Normal');
prachConfig=struct('Format',0,'SeqIdx',0,'PreambleIdx',0,'CyclicShiftIdx',0,'HighSpeed',0,'TimingOffset',0,'FreqIdx',0,'FreqOffset',8);

addpath('../../build/srslte/lib/phch/test')

NULRB=[100];

% FreqIdx, FreqOffset and TimeOffset need to be tested

for n_rb=1:length(NULRB)
    for format=0;
        for seqIdx=7:17:237
            for preambleIdx=0:23:63
                for CyclicShift=1:3:15
                   fprintf('RB: %d, format %d, seqIdx: %d, Cyc=%d Idx=%d\n',NULRB(n_rb),format,seqIdx, CyclicShift, preambleIdx);
                    %for hs=0:1
                    hs=0;
                        ueConfig.NULRB=NULRB(n_rb);
                        prachConfig.Format=format;
                        prachConfig.SeqIdx=seqIdx;
                        prachConfig.PreambleIdx=preambleIdx;
                        prachConfig.CyclicShiftIdx=CyclicShift;
                        prachConfig.HighSpeed=hs;
                        prachConfig.FreqIdx=0;
                        prachConfig.FreqOffset=0;
                        lib=srslte_prach(ueConfig,prachConfig);
                        
                        [mat, info]=ltePRACH(ueConfig,prachConfig);
                        err=max(abs(mat-lib));
                        if (err > 2*10^-3)
                            disp(err)    
                            a=1:100;
                            plot(a,real(lib(a)),a,real(mat(a)))
                            error('Error!');
                        end
                    %end
                end
            end
        end
    end
end

% 
% disp(info)
 n=1:length(mat);
% plot(abs(double(mat)-double(lib)))
flib=fft(lib(199:end),1536);
fmat=fft(mat(199:end),1536);
n=1:1536;
plot(n,real(flib(n)),n,real(fmat(n)))
