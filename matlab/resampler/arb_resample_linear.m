function out = arb_resample_linear(sig, rate)

N = 32;
M = 8;
step = (1/rate)*N;

load('arb_filter.mat');

figure;plot(Num);title('Filter impulse response');
[h,w] = freqz(Num, 1);
figure;plot(20*log10(abs(h)));title('Filter freq response');

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
    filt_reg2 = poly(mod(index+1,N)+1, :);
    res1 = sig_reg*filt_reg1';
    res2 = sig_reg*filt_reg2';
    
    if index+1 == 32
        res = res1;
    else
        res = res1 + (res2-res1)*frac;
    end
    
    out = [out res];
    
    acc = acc+step;
    index = fix(acc);
    while index >= N
        acc = acc - N;
        index = index - N;
        k = k+1;
    end
    frac = abs(acc-index);
end
