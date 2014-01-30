function [ ifo ] = ifo_pss( r_pss, x_pss)

k=1;
v=-31:31;
c=zeros(length(v),1);
for i=v
    c(k) = ifo_pss_corr(i, r_pss, x_pss);
    k=k+1;
end
[m i]=max(c);
ifo=v(i);
plot(v,c); 

