function [peaks] = find_peaks(x, N_id_2, fft_size)

flen=4800*(ceil(fft_size/64));

n=floor(length(x)/flen)*flen;
xf=reshape(x(1:n),flen,[]);

[n m] = size(xf);

peaks=zeros(m,1);
for i=1:m
    [w, peaks(i)]= find_pss2(xf(:,i),N_id_2,fft_size);
end
