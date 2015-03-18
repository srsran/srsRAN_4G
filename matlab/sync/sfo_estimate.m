function [ sfo sfo_v ] = srslte_sfo_estimate( fs, T )


nanfs=fs(~isnan(fs));
idx=find(~isnan(fs));
sfo_v = zeros(length(nanfs)-1,1);
for i=2:length(nanfs)
    if (abs(nanfs(i)-nanfs(i-1))<9000)
        sfo_v(i-1)=(nanfs(i)-nanfs(i-1))/T/(idx(i)-idx(i-1));
    else
        sfo_v(i-1)=sfo_v(i-2);
    end
end

sfo = mean(sfo_v);



