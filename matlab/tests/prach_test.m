clear 
ueConfig=struct('NULRB',6,'DuplexMode','FDD','CyclicPrefix','Normal');
prachConfig=struct('Format',0,'SeqIdx',0,'PreambleIdx',0,'CyclicShiftIdx',0,'HighSpeed',0,'TimingOffset',0,'FreqIdx',0,'FreqOffset',0);

addpath('../../debug/lte/phy/lib/phch/test')

NULRB=[6 15 25 50 100];

% FreqIdx, FreqOffset and TimeOffset need to be tested

for n_rb=1:length(NULRB)
    for format=0:3
        for seqIdx=0:837
            fprintf('format %d, seqIdx: %d\n',format,seqIdx);
            for preambleIdx=0:63
                for CyclicShift=0:15
                    %for hs=0:1
                    hs=0;
                        ueConfig.NULRB=NULRB(n_rb);
                        prachConfig.Format=format;
                        prachConfig.SeqIdx=seqIdx;
                        prachConfig.PreambleIdx=preambleIdx;
                        prachConfig.CyclicShiftIdx=CyclicShift;
                        prachConfig.HighSpeed=hs;

                        [mat, info]=ltePRACH(ueConfig,prachConfig);

                        lib=liblte_prach(ueConfig,prachConfig);
                         err=mean(abs(mat(:)-lib(:)));
                         if (err > 10^-3)
                             disp(err)    
                             error('Error!');
                         end
                  %  end
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
