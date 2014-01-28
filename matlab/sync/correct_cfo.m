function [ y eps] = correct_cfo( x )

xf=reshape(x,9600,[]);
yf=zeros(size(xf));
[m n]=size(xf);

for i=1:n
   [fs eps(i)]=find_pss(xf(:,i),0,false);
   yf(:,i)=xf(:,i).*exp(-1i.*(1:length(xf(:,i)))'.*2*pi*eps(i)/128);
end

y=reshape(yf,1,[]);

end