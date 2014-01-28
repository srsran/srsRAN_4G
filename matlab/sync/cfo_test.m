%clear;
M=1000;
sym_len=128;
x=lte(1:M*15360*sym_len/2048*2000/1536);
%x=read_complex('../../../eclipse_osldlib/test.dat');
%y=resample(x,99839996,100000000);

input=resample(x,1536,2000);
%input=x;
%input=y(1:M*15360*sym_len/2048);
%input=resample(x,3840000,1920000);

cp0_len=160*sym_len/2048;
cp1_len=144*sym_len/2048;

slots=reshape(input,15360*sym_len/2048,[]);
[n m]=size(slots);

cfo=zeros(m,1);
output=zeros(size(input));
for i=1:m
    cfo(i)=cfo_estimate(slots(:,i),7,sym_len,cp1_len,cp1_len);
    t=(i-1)*n+1:i*n;
    %output(t)=input(t).*exp(-1i*2*pi*cfo(i)*t/sym_len);
end

plot(cfo)
