close all 
clear all

up = 24;
down = 25;
d_rate = up/down;
Fs = 100;          % Arbitrary sample rate (used for displays)
Fsin = 1;

% Create a sine wave
t = 0:1/Fs:1-1/Fs;
sig = sin(2*pi*t);

out = arb_resample_nearest(sig,d_rate);

% matlab resample for comparison
out2 = resample(sig, up, down);
figure;hold on;title('Ours and matlabs');
l = min(length(out), length(out2));
stem(out(1:l));
stem(out2(1:l), 'r', 'filled');
diff = out2(1:l)-out(1:l);
figure;plot(diff);title('Difference between ours and matlabs');

figure; hold on;title('Original and resampled - no time scaling');
stem(sig);
stem(out, 'r', 'filled');

% Time align and plot
figure;hold on;title('Original and resampled - with time scaling');
stem((1:75)/Fs,real(sig(1:75)));  
stem((1:72)/(Fs*d_rate),real(out(1:72)),'r','filled');
xlabel('Time (sec)');ylabel('Signal value');