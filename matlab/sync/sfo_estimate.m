function [ sfo ] = sfo_estimate( fs, T )

sfo = 0;
for i=2:length(fs)
    sfo=sfo + (fs(i)-fs(i-1))/length(fs)/T;
end

