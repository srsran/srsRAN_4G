enb = lteTestModel('1.1','5MHz');
Ntrials = 1; 
SNR_values =-20;%linspace(-18,-10,8);

tx_offset = randi(50,Ntrials,1);
diff=zeros(size(SNR_values));
diff_lt=zeros(size(SNR_values));

tx_signal = lteTestModelTool(enb); 
tx_power = mean(tx_signal.*conj(tx_signal));

corrcfg.PSS='On';
corrcfg.SSS='On';
corrcfg.CellRS='Off';

addpath('../../debug/lte/phy/lib/sync/test')

for snr_idx=1:length(SNR_values)
    SNRdB = SNR_values(snr_idx);
    rx_offset = zeros(size(tx_offset));
    rx_offset_lt = zeros(size(tx_offset));
    for i=1:Ntrials
        SNR = 10^(SNRdB/10);    % Linear SNR  
        tx = [zeros(tx_offset(i),1); tx_signal];
        N0  = tx_power/(sqrt(2.0)*SNR);
        noise = N0*complex(randn(size(tx)), randn(size(tx)));  % Generate noise
        rx=noise+tx;
        [rx_offset(i),corr] = lteDLFrameOffset(enb,rx,corrcfg);
        [rx_offset_lt(i),corr_lt] = srslte_pss(enb,rx);
    end
    diff(snr_idx)=sum(abs(rx_offset-tx_offset));
    diff_lt(snr_idx)=sum(abs(rx_offset_lt-tx_offset));
    disp(SNRdB)
end

if (Ntrials == 1)
    len=1:length(corr)-rx_offset(i)-3840;
    len2=rx_offset(i)+1+3840:length(corr);
    plot(len,corr(len)/max(corr(len)),...
        len,abs(corr_lt(len2))/max(abs(corr_lt(len2))));
else
    plot(SNR_values,diff,SNR_values,diff_lt);
end

