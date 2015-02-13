fprintf('static uint8_t M_basis_seq_pucch[20][13]={\n');
for i=1:20
    fprintf('\t\t{');
    for j=1:12
        fprintf('%d, ',y(i,j));
    end
    fprintf('%d},\n',y(i,13));
end
fprintf('\t\t};\n');
