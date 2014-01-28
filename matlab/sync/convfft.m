function [out] = convfft(z1,z2)
%CONVFFT FFT-based convolution and polynomial multiplication.
%    C = CONVFFT(A, B) convolves vectors A and B.  The resulting
%    vector is length LENGTH(A)+LENGTH(B)-1.
%    If A and B are vectors of polynomial coefficients, convolving
%    them is equivalent to multiplying the two polynomials.
%
% Please contribute if you find this software useful.
% Report bugs to luigi.rosa@tiscali.it
%
%*****************************************************************
% Luigi Rosa
% Via Centrale 27
% 67042 Civita di Bagno
% L'Aquila --- ITALY 
% email  luigi.rosa@tiscali.it
% mobile +39 340 3463208 
% http://utenti.lycos.it/matlab
%*****************************************************************
%


z1x=size(z1,1);
z1y=size(z1,2);
z2x=size(z2,1);
z2y=size(z2,2);
if (~isa(z1,'double'))||(~isa(z2,'double'))||(ndims(z1)>2)||(ndims(z2)>2)
    disp('Error: input vector must be unidimensional double array');
    out=[];
    return;
else
    if ((z1x>1)&&(z1y>1)) || ((z2x>1)&&(z2y>1))
        out=[];
        disp('Error: input vectors are double matrices');
        return;
        
    else
        
        if (z1x==1)&&(z1y>1)    
            z1=z1';
            z1x=z1y;
        end
        
        
        if (z2x==1)&&(z2y>1)            
            z2=z2';
            z2x=z2y;
        end
        
        
        if (any(any(imag(z1))))||(any(any(imag(z2))))
            out=(ifft(fft(z1,z1x+z2x-1).*fft(z2,z1x+z2x-1)));
        else
            out=real(ifft(fft(z1,z1x+z2x-1).*fft(z2,z1x+z2x-1)));
        end
    end
end