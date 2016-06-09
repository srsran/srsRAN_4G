function [ s ] = bin2hex( in )
%BIN2HEX Summary of this function goes here
%   Detailed explanation goes here

a=reshape(in,8,[]);
t=size(a);
s=cell(1,t(2));
nbit=2.^(7:-1:0);
for i=1:t(2)
    s{i}=dec2hex(sum(nbit'.*double(a(:,i))));
end
for i=1:t(2)
    if (length(s{i})==1)
        fprintf('0%s ',s{i})        
    else
        fprintf('%s ',s{i})
    end
end
fprintf('\n');
end

