function [ out ] = write_c_struct_complex( filename, varname, x)

    [tidin msg]=fopen(filename,'wt');
    if (tidin==-1)
        fprintf('error opening %s: %s\n',filename, msg);
        out=[];
        return
    end
    fprintf(tidin, 'float %s[%d]={%.9g,%.9g',varname,2*length(x),real(x(1)),imag(x(1)));
    fprintf(tidin, ',\n %.9g,%.9g',real(x(2:end)),imag(x(2:end)));
    fprintf(tidin, '};\n');
    fclose(tidin);
end

