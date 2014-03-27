F=100000./(1:100);
PQ=(1:50);

PRB=50;
Mmax=1024;
N=PRB*12;

Fmin=N*15*1.1;
Fs=F(F>Fmin);

for i=length(Fs):-1:1
   for p=PQ
       for q=PQ
           if (mod(p/q*Fs(i),15)==0 && p/q*Fs(i)/15<Mmax)
                fprintf('found Fs=%f p=%d, q=%d, M=%d\n',Fs(i),p,q,p/q*Fs(i)/15);
                return
           end
       end
   end    
end