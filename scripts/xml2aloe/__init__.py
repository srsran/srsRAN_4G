import shutil
import os

def MakeModule(m):
    dir = os.path.dirname(__file__)

    tdir = os.path.join(dir, 'template')
    ddir = tdir + m.directory_name
    print 'Creating new directory ' + ddir
    
    shutil.copytree(tdir,ddir)
    
    WriteAloeC(m,tdir + '/src/template.c',\
                    ddir + '/src/' + m.name + '.c')
    WriteAloeH(m,tdir + '/src/template.h',\
                    ddir + '/src/' + m.name + '.h')
    

def WriteInitParamsGet(m, f):
    for p in m.init_params:
        f.write('\tif (param_get_' + p['type'] + '_name("' + p['name'] + '", &' + p['variable']+ ')) {\n'
                    '\t\t' + p['variable'] + ' = ' + str(p['default']) + ';\n'\
                '\t}\n')
    
def WriteInputParamsHandlers(m, f):
    for p in m.input_params:
        f.write('pmid_t ' + p['name'] + '_id;\n')

def WriteInputParamsId(m, f):
    for p in m.input_params:
        f.write('\t' + p['name'] + '_id = param_id("' + p['name'] + '");\n')
        
def WriteInputParamsGet(m, f):
    for p in m.input_params:
        f.write('\tif (param_get_' + p['type'] + '(' + p['name'] + '_id, &' + p['variable'] + ') != 1) {\n')
        if p['default'] == None:
            f.write('\t\t' + r'moderror("Error getting parameter ' + p['name'] + '\\n");' + '\n')
            f.write('\t\treturn -1;\n\t}\n')                      
        else:
            f.write('\t\t' + r'modinfo("Parameter ' + p['name'] + ' not found. Setting to default (' + p['default'] + ')\\n");' + '\n')
            f.write('\t\t' + p['variable'] + ' = ' + p['default'] + ';\n\t}\n')
            
    
def WriteAloeC(m, input, output):
    with open(input,'r') as f:
        newlines = []
        for line in f.readlines():
            newlines.append(line.replace('-name-', m.name))
    with open(output, 'w') as f:
        for line in newlines:

            if '--input_parameters_handlers--' in line:
                WriteInputParamsHandlers(m, f)
            elif '--input_parameters_getid--' in line:
                WriteInputParamsId(m, f)
            elif '--init_parameters--' in line:
                WriteInitParamsGet(m, f)
            elif '--input_parameters--' in line:
                WriteInputParamsGet(m, f)
            else: 
                f.write(line)

def WriteAloeH(m, input, output):
    with open(input,'r') as f:
        newlines = []
        for line in f.readlines():
            if '-typeinput-' in line:
                if m.nof_inputs > 0: 
                    newlines.append(line.replace('-typeinput-', m.input_type))
            elif '-numinputs-' in line:
                newlines.append(line.replace('-numinputs-', str(m.nof_inputs)))
            elif '-sizeinput-' in line:
                if m.nof_inputs > 0: 
                    newlines.append(line.replace('-sizeinput-', m.input_size))
                else:                                        
                    newlines.append(line.replace('-sizeinput-', '0'))
            elif '-typeoutput-' in line:
                if m.nof_outputs > 0: 
                    newlines.append(line.replace('-typeoutput-', m.output_type))
            elif '-numoutputs-' in line:
                newlines.append(line.replace('-numoutputs-', str(m.nof_outputs)))
            elif '-sizeoutput-' in line:
                if m.nof_outputs > 0: 
                    newlines.append(line.replace('-sizeoutput-', m.output_size))
                else:
                    newlines.append(line.replace('-sizeoutput-', '0'))
            else:
                 newlines.append(line)
                  
    with open(output, 'w') as f:
        for line in newlines:          
              f.write(line)      
                    
               