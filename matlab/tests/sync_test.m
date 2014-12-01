enb = lteTestModel('1.1','5MHz');
Ntrials = 1; 
SNR_values =-10;%linspace(-18,-10,8);

tx_offset = randi(50,Ntrials,1);
tx_offset = 50;
diff=zeros(size(SNR_values));

tx_signal = lteTestModelTool(enb); 
tx_power = mean(tx_signal.*conj(tx_signal));

for snr_idx=1:length(SNR_values)
    SNRdB = SNR_values(snr_idx);
    rx_offset = zeros(size(tx_offset));
    for i=1:Ntrials
        SNR = 10^(SNRdB/10);    % Linear SNR  
        tx = [zeros(tx_offset(i),1); tx_signal];
        N0  = tx_power/(sqrt(2.0)*SNR);
        noise = N0*complex(randn(size(tx)), randn(size(tx)));  % Generate noise
        rx=noise+tx;
        [rx_offset(i),corr] = lteDLFrameOffset(enb,rx);
    end
    diff(snr_idx)=sum(abs(rx_offset-tx_offset));
    disp(SNRdB)
end

if (Ntrials == 1)
    plot(corr)
else
    plot(SNR_values,diff);
end

