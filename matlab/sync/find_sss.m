function [ m1 m2 out Zprod0 Zprod1] = find_sss( y, Nid_2,c0,c1)

    y=y((960-2*137+1):(960-137-9));
    yf=fft(y,128);
    y=[yf(98:128) yf(2:32)];
   

    n=length(y);
    Y0=y(1:2:n);
    Y1=y(2:2:n);
    
    Z0=Y0.*c0;
    nz=length(Z0);
    
    sm0=zeros(31,31);
    sm1=zeros(31,31);
    zm0=zeros(31,31);

    for i=1:31
        [sm0(i,:) zm0(i,:)]=compute_m0(i-1);
    end
      
    Zprod0=Z0(2:nz).*conj(Z0(1:(nz-1)));

    sum0=zeros(31,1);
    for m=1:31
        for i=2:31
            sum0(m)=sum0(m)+Z0(i)*conj(Z0(i-1))*sm0(m,i)*conj(sm0(m,i-1));
        end
    end

    [mi1 i1]=max(abs(sum0));

    Z1=Y1.*c1.*zm0(i1,:);

    Zprod1=Z1(2:nz).*conj(Z1(1:(nz-1)));

    sum1=zeros(31,1);
    for m=1:31
        for i=2:31
            sum1(m)=sum1(m)+Z1(i)*conj(Z1(i-1))*sm0(m,i)*conj(sm0(m,i-1));
        end
    end
    [mi2 i2]=max(abs(sum1));

    m1=i1;
    m2=i2;
    out=[sum0; sum1];
end

