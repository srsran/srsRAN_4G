function [ out ] = write_complex( filename, x)
%READ_COMPLEX Summary of this function goes here
%   Detailed explanation goes here

    [tidin msg]=fopen(filename,'w');
    if (tidin==-1)
        fprintf('error opening %s: %s\n',filename, msg);
        out=[];
        return
    end

    if (isreal(x))
        y=x;
    else
        i=1:2:2*length(x);
        y(i)=real(x);
        y(i+1)=imag(x);
    end
    
    fwrite(tidin,y,'single');
end

