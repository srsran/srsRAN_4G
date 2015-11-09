clear;
sym_len=128;
hflen = (sym_len/128)*1920*5;
samp_rate = (sym_len/128)*1920000;
N_id_2=0;
input=read_complex('../../debug/lte_signal.dat', hflen*240);

addpath('../../debug/srslte/lib/sync/test')

off=100;
halfframes=reshape(input(hflen-off+1:end-hflen-off),hflen,[]);
[n m]=size(halfframes);

fo=linspace(-30000,30000,m);

nreal=1;

cfdl=struct('NDLRB',6,'CyclicPrefix','Normal','NCellID',0,'CellRefP',1,'DuplexMode','FDD');
cfo=zeros(m,3);
toffset=zeros(m,2);
for i=1:m
    x = halfframes(:,i);
    t = (0:n-1).'/samp_rate;
    x = x .* exp(1i*2*pi*fo(i).*t);
    cfo_=zeros(nreal,2);
    idx_=zeros(nreal,2);
    for j=1:nreal
        y=awgn(x,5);
        cfo_(j,1)              = lteFrequencyOffset(cfdl,y)/15000;
        [idx_(j,1), cfo_(j,2)] = find_pss(y,N_id_2);
        [idx_(j,2), corr]      = srslte_cp_synch(cfdl,y);
        cfo_(j,3)              = -angle(corr(idx_(j,2)+1))/2/pi;
        idx_(j,1) = idx_(j,1)-961;
    end
    cfo(i,:)=mean(cfo_,1);
    toffset(i,:)=mean(idx_,1);
end

error=abs(cfo-repmat(fo',1,3)/15000);
semilogy(fo/15000,error)
%plot(fo/15000,toffset)
legend('Matlab','PSS-based','CP-based')
