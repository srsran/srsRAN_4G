function [ out ] = read_complex( filename, count )
%READ_COMPLEX Summary of this function goes here
%   Detailed explanation goes here

    [tidin msg]=fopen(filename,'r');
    if (tidin==-1)
        fprintf('error opening %s: %s\n',filename, msg);
        out=[];
        return
    end

    if (nargin==1) 
        count=inf;
    end

    x=fread(tidin,2*count,'single');
    i=1:2:length(x);
    out=x(i)+x(i+1)*1i;
end

