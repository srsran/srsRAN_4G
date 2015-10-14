function [ out ] = write_uchar( filename, x)
%READ_COMPLEX Summary of this function goes here
%   Detailed explanation goes here

    [tidin msg]=fopen(filename,'w');
    if (tidin==-1)
        fprintf('error opening %s: %s\n',filename, msg);
        out=[];
        return
    end
    
    fwrite(tidin,x,'uint8');
end

