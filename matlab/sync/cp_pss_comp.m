function cp_pss_comp(x,N_id_2)

[ fs eps p_m w2] = find_pss( x, N_id_2);
lambda=zeros(1,length(x)-138);
for theta=1:length(lambda)
    lambda(theta)=cp_corr(x,theta,128,9,0);
end
plot(1:length(w2),abs(w2)/max(abs(w2)),1:length(lambda),abs(lambda)/max(abs(lambda)))