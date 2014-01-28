N=128; %128 subcarries
M=16; %QAM order
cp=9; %length of the cyclic prefix... Is increasing the cyclic prefix size gonna increase the efficiency?
scale = 1/sqrt(10);
hMod = modem.qammod(M); %QAM Modulator
hDemod = modem.qamdemod(hMod);  %QAM demodulator
loops = 10;
SNR =0:5:35;
t1= cputime ;
% transmited signal. Contains N data points ranging from 0 to M-1
ber=zeros(5,length(SNR));
%% Creating the Rayleigh Multipath Channels 
Ch = rayleighchan(1/1000,10);
Ch.ResetBeforeFiltering = 0;
sig = 1i*ones(loops,1);
h1 = filter(Ch,sig);
h2 = 0.1072*filter(Ch,sig);
h3 = 0.0120*filter(Ch,sig);
h4 = 0.0052*filter(Ch,sig);
% Delay Values 
l1 = 4;
l2 = 7;
l3= 16;
%%
ofdm_cp=[];
    %tx=transmited_data;
    for ik=1:loops%number of loops
        tx = randi([0 M-1],1,N);  % generate random data
        sig=modulate(hMod, tx)*scale; % Modulate QAM modulated signal, devide by the square root of 10 to bring the average power of the signal to 1
        ofdm=sqrt(N).*ifft(sig,N); % generate OFDM signal IFFT on the parrellel data,multiply by sqrt(N) to adjust to the matlab computation ,
        ofdm_cp = [ofdm_cp ofdm(N-cp+1:N) ofdm]; % Add cyclic prefix
       
      end
  
  