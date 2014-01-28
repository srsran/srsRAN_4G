function [ fs ] = check_pss( x, N_id_2)
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
for i=1:nf-1
    [fs(i) cfo(i) m_p(i)]=find_pss(xf(:,i),N_id_2,false);
    if (fs(i)<0)
        j=0;
    end
%    cfo2(i) = cfo_estimate_cp(xf(fs(i)+960:fs(i)+2*960,i),7,128,10,9);
end

sfo=sfo_estimate(fs, 5/1000);

subplot(1,3,1)
plot(1:nf,fs)
legend('PSS-based');
subplot(1,3,2)
plot(1:nf, cfo, 1:nf, cfo2)
legend('PSS-based','CP-based');
if (nf > 0)
    axis([0 nf -0.5 0.5])
end
subplot(1,3,3)
plot(m_p)
fprintf('pss_mean=%g, pss_var=%g, cp_mean=%g, cp_var=%g m_p=%g sfo=%g Hz\n',mean(cfo),var(cfo), mean(cfo2), var(cfo2), mean(m_p), sfo)
end


