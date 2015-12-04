nof_cb=11;
TBS=61664;
K=5632; % Only supporting 1 K for now
rv=0;
chs.Modulation='64QAM';
chs.NLayers=1;
% cws must be a vector of size TBS in workspace containing the output of the
% descrambler
rmout_mat=lteRateRecoverTurbo(cws{1},TBS,rv,chs);
scale=700;
%path='../../build/srslte/lib/phch/test';
path='.';
error=zeros(nof_cb,3*K+12);
rmout_lib=zeros(nof_cb,3*K+12);
for i=0:nof_cb-1   
    filename=sprintf('%s/rmout_%d.dat',path,i);
    x=read_int16(filename);
    rmout_lib(i+1,:) = reshape(reshape(x,3,[])',[],1); 
    error(i+1,:)=abs(transpose(rmout_mat{i+1})-rmout_lib(i+1,:)/scale);
end

plot(reshape(error',1,[]))