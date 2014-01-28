osld-lib
========

OSLD-lib is a free and open-source LTE library for SDR UE and eNodeB. The library does not rely on any external dependencies or frameworks. 


The project contains a set of Python tools for the automatic code generation of modules for popular SDR frameworks, including GNURadio, ALOE++, IRIS, and OSSIE. These tools are easy to use and adapt for generating targets for specific platforms or frameworks. 

The DSP modules are based on OSLD (https://github.com/flexnets/aloe). 

## Examples

Currently, only PSS/SSS decoding is available:


'
mkdir build
cd build
cmake ../
make
examples/synch_test -i ../lte_signal.txt -N 0 
'

Searches for an N_id_2 PSS/SSS signal in the provided file. 
