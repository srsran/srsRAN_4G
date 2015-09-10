
clear
blen=5184;
SNR_values_db=linspace(-1.3,-0.7,6);
Nrealizations=600;

addpath('../../build/srslte/lib/fec/test')

errors1=zeros(1,length(SNR_values_db));
errors2=zeros(1,length(SNR_values_db));
for snr_idx=1:length(SNR_values_db)
    SNRdB = SNR_values_db(snr_idx);             % Desired SNR in dB
    SNR = 10^(SNRdB/20);    % Linear SNR  

    for i=1:Nrealizations
        Data = randi(2,blen,1)==1;
        codedData = lteTurboEncode(Data);

        codedsymbols = 2*double(codedData)-1;
        
        %% Additive Noise
        N0 = 1/SNR;

        % Create additive white Gaussian noise
        noise = N0*randn(size(codedsymbols));   
        
        noisysymbols = noise + codedsymbols; 
        
        decodedData = lteTurboDecode(noisysymbols);
        interleavedSymbols = reshape(reshape(noisysymbols,[],3)',1,[]);
        [decodedData2] = srslte_turbodecoder(interleavedSymbols);
        
        errors1(snr_idx) = errors1(snr_idx) + any(decodedData ~= Data);
        errors2(snr_idx) = errors2(snr_idx) + any(decodedData2 ~= Data);
    end
    
    fprintf('SNR: %.2f BLER: %f-%f\n', SNR_values_db(snr_idx), ...
        errors1(snr_idx)/Nrealizations, errors2(snr_idx)/Nrealizations);
end

if (length(SNR_values_db) > 1)
    semilogy(SNR_values_db, errors1/Nrealizations, ...
             SNR_values_db, errors2/Nrealizations)
    grid on
    xlabel('SNR (dB)')
    ylabel('BLER')
    legend('Matlab','srsLTE');
else
    disp(errors1);
    disp(errors2);
end