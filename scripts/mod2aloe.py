#
# Copyright 2012-2013 The libLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the libLTE library.
#
# libLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# libLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#


#!/usr/bin/env python

from module import Module
from xml2aloe import MakeModule
import sys, os, getopt

argv = sys.argv
input_file = None
output_dir = None
try:
   opts, args = getopt.getopt(argv,"hi:o:",["input_file=","output_dir="])
except getopt.GetoptError:
   print argv[0] + ' -i <input_file> -o <output_dir>'
   sys.exit(2)
for opt, arg in opts:
   if opt == '-h':
      print argv[0] + ' -i <input_file> -o <output_dir>'
      sys.exit()
   elif opt in ("-i", "--input_file"):
      input_file = arg
   elif opt in ("-o", "--output_dir"):
      output_dir = arg

if input_file == None or output_dir == None:
    print argv[0] + ' -i <input_file> -o <output_dir>'
    sys.exit(2)

filename=os.path.basename(input_file).split('.')[0]
 
print filename + '\n'
print input_file + '\n'
print output_dir + '\n'
 #m = Module("binsource")
 
 #m.readHeader(input_file)
 #MakeModule(m,output_dir)
 #print m.toString()
 
