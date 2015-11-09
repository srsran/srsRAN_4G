function[a]=lte_pss_zc(cell_id)
% Function returns 1 out of 3 possible Zadoff-Chu sequences used in LTE.

Nzc=62;
u=0;
if cell_id==0
    u=25;
end
if cell_id==1
    u=29;
end
if cell_id==2
    u=34;
end

a = zeros(Nzc,1);
for n=0:30
    a(n+1)=exp(complex(0,-1)*pi*u*n*(n+1)/63);
end    
for n=31:61
    a(n+1)=exp(complex(0,-1)*pi*u*(n+1)*(n+2)/63);
end    

end
