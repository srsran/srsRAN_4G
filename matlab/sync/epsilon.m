function [ eps fs] = epsilon( x )

xf=reshape(x,19200,[]);

[m n]=size(xf);
eps=zeros(n,1);
fs=zeros(n,1);

for i=1:n
   [fs(i) eps(i)]=find_pss(xf(:,i),0,false);
end


end