
SNR_values = linspace(-6,4,10);
Npackets = 200; 
CFO=4/15;
m0=7;
m1=10;
%m0=26;
%m1=21;

recordedWaveform = x;
if (~isempty(recordedWaveform))
    Npackets = floor(length(recordedWaveform)/19200)-1;
    SNR_values = 0;
end

error = zeros(6,length(SNR_values));

enb = struct('NCellID',2,'NSubframe',0,'NDLRB',6,'CellRefP',1,'CyclicPrefix','Normal','DuplexMode','FDD');
sss=lteSSS(enb);

cfg.Seed = 2;                  % Random channel seed
cfg.NRxAnts = 1;               % 1 receive antenna
cfg.DelayProfile = 'ETU';      % EVA delay spread
cfg.DopplerFreq = 144;         % 120Hz Doppler frequency
cfg.MIMOCorrelation = 'Low';   % Low (no) MIMO correlation
cfg.NTerms = 16;               % Oscillators used in fading model
cfg.ModelType = 'GMEDS';       % Rayleigh fading model type
cfg.InitPhase = 'Random';      % Random initial phases
cfg.NormalizePathGains = 'On'; % Normalize delay profile power 
cfg.NormalizeTxAnts = 'On';    % Normalize for transmit antennas       % Initialize at time zero

[s, c0, c1] = get_sc(mod(enb.NCellID,3));

subframe = lteDLResourceGrid(enb);
sssSym = lteSSS(enb);
sssInd = lteSSSIndices(enb);
subframe(sssInd) = sssSym;
N_id_1 = floor(enb.NCellID/3);

[txWaveform,info] = lteOFDMModulate(enb,subframe); 
cfg.SamplingRate = info.SamplingRate;
fftSize = info.Nfft;


addpath('../../debug/lte/phy/lib/sync/test')

for snr_idx=1:length(SNR_values)
    SNRdB = SNR_values(snr_idx);
    for i=1:Npackets
        %% Noise Addition
        SNR = 10^(SNRdB/10);    % Linear SNR  

        if (isempty(recordedWaveform))
            cfg.InitTime = i*(10^-3);              
            [rxWaveform, info]= lteFadingChannel(cfg,txWaveform);
            rxWaveform = txWaveform; 

            % Add CFO
            freq = CFO/double(fftSize);
            rxWaveform = rxWaveform.*exp(1i*2*pi*freq*(1:length(txWaveform))');

            N0 = 1/(sqrt(2.0*enb.CellRefP*double(fftSize))*SNR);
            noise = N0*complex(randn(size(rxWaveform)), randn(size(rxWaveform)));  % Generate noise

            rxWaveform = rxWaveform + noise;
        else
            rxWaveform = recordedWaveform(i*19200+1:(i+1)*19200); 
        end
        
        offset = lteDLFrameOffset(enb,rxWaveform);
        offsetVec(i)=offset;
        rxWaveform = [rxWaveform(1+offset:end,:); zeros(offset,1)];

        subframe_rx = lteOFDMDemodulate(enb,rxWaveform,1);
        
        sss_rx = subframe_rx(lteSSSIndices(enb));
        sss0=sss_rx(1:2:end);
        sss1=sss_rx(2:2:end);

        beta0=sss0.*c0';
        beta1=sss1.*c1';

        corr0=zeros(31,1);
        for m=1:31
            corr0(m)=sum(beta0.*s(m,:)');    
        end
        corr0=abs(corr0).^2;
        [m, idx]=max(corr0);
        
        error(1,snr_idx) = error(1,snr_idx) + ((idx ~= m0 && idx ~= m1));
        
        M=2;
        Nm=10;

        corr2=zeros(31,1);
        for m=1:31
            for j=0:M
                idx=1+j*Nm:(j+1)*Nm;
                corr2(m)=corr2(m)+abs(sum(beta0(idx).*s(m,idx)')).^2;
            end
        end
        [m, idx]=max(corr2);
        
        error(2,snr_idx) = error(2,snr_idx) + ((idx ~= m0 && idx ~= m1));

        corr3=zeros(31,1);
        for m=1:31
            corr3(m)=abs(sum(beta0(2:end).*conj(beta0(1:end-1)).*transpose(s(m,2:end).*conj(s(m,1:end-1))))).^2;            
        end
        [m, idx]=max(corr3);
        
        error(3,snr_idx) = error(3,snr_idx) + ((idx ~= m0 && idx ~= m1));
        
        % srsLTE results
        [n,sf_idx,lt_corr0]=srslte_sss(enb,rxWaveform,'full');
        [m, idx]=max(lt_corr0);
        error(4,snr_idx) = error(4,snr_idx) + ((idx ~= m0 && idx ~= m1));

        [n,sf_idx,lt_corr2]=srslte_sss(enb,rxWaveform,'partial');
        [m, idx]=max(lt_corr2);
        error(5,snr_idx) = error(5,snr_idx) + ((idx ~= m0 && idx ~= m1));
        
        [n,sf_idx,lt_corr3]=srslte_sss(enb,rxWaveform,'diff');
        [m, idx]=max(lt_corr3);
        error(6,snr_idx) = error(6,snr_idx) + ((idx ~= m0 && idx ~= m1));
        
    end
end

if (length(SNR_values) > 1) 
    plot(SNR_values,1-error/Npackets)
    legend('Full','Partial','Differential','Full-lt','Partial-lt','Differential-lt')
    grid on
else
    e=error/Npackets;
    fprintf('Full (mt/lt): \t%f/%f\n',e(1),e(4));
    fprintf('Partial (mt/lt):%f/%f\n',e(2),e(5));
    fprintf('Diff (mt/lt): \t%f/%f\n',e(3),e(6));
end





