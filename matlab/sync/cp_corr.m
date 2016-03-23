function [ lambda ] = cp_corr( x, theta, N, L)

l0=sum(x(theta:theta+L-1).*conj(x(theta+N:theta+L+N-1)));
l1=0;
for i=theta:theta+L-1
    l1=l1+abs(x(i))^2+abs(x(i+N))^2;
end

lambda=l1;%2*abs(l0)-rho*l1;
