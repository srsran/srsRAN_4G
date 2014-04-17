function out = arb_resample_nearest(sig, rate)

N = 32;
M = 8;
step = (1/rate)*N;

load('arb_filter.mat');

figure;plot(Num);title('Filter impulse response');
[h,w] = freqz(Num, 1);
figure;plot(20*log10(abs(h)));title('Filter frequency response');

% Create polyphase partition
poly = reshape(Num, N, M);

% Filter
sig = [zeros(1,(M/2)-1) sig];
k=0;
acc=0;
index=0;
frac=0;
out = [];
while k < length(sig)-M
    sig_reg = fliplr(sig(k+1:k+M));
    filt_reg1 = poly(index+1, :);
    res = sig_reg*filt_reg1';
    
    out = [out res];
    
    acc = acc+step;
    index = round(acc);
    while index >= N
        acc = acc - N;
        index = index - N;
        k = k+1;
    end
end