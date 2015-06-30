function [ values, tti] = read_trace_uint( filename, count )

    [tidin msg]=fopen(filename,'r');
    if (tidin==-1)
        fprintf('error opening %s: %s\n',filename, msg);
        out=[];
        return
    end

    if (nargin==1) 
        count=inf;
    end

    x=fread(tidin,2*count,'uint');
    i=1:2:length(x);
    tti=x(i);
    values=x(i+1);
end

