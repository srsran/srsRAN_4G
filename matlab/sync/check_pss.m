function [ fs ] = check_pss( x, N_id_2, threshold)
%CHECK_PSS Summary of this function goes here
%   Detailed explanation goes here
flen=9600;
n=length(x);
nf=floor(n/flen);

xf=reshape(x(1:nf*flen),flen,[]);

fs=zeros(nf,1);
cfo=zeros(nf,1);
cfo2=zeros(nf,1);
m_p=zeros(nf,1);
for i=1:nf
    [fs(i) cfo(i) m_p(i)]=find_pss(xf(:,i),N_id_2,false, threshold);
end

fs=fs+960;
[sfo sfo_v]=srslte_sfo_estimate(fs, 5/1000);

subplot(1,3,1)
plot(1:nf,fs)
subplot(1,3,2)
plot(1:nf, cfo)
if (nf > 0)
    axis([0 nf -0.5 0.5])
end
subplot(1,3,3)
plot(m_p)
fprintf('cfo_mean=%g Hz, cfo_std=%g Hz, m_p=%g sfo=%g Hz\n',15000*nanmean(cfo),15000*nanstd(cfo), nanmean(m_p), sfo)
end


