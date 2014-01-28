function [ fs ] = sym_sync_cp( x, s )

lambda=zeros(1,length(x)-138);
for theta=1:length(lambda)
    lambda(theta)=cp_corr(x,theta,128,9,s);
end

subplot(1,2,1)
plot(abs(lambda))
subplot(1,2,2)
plot(angle(lambda))
[m i] = max(abs(lambda))

