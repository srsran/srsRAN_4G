#!/usr/bin/env python

from module import Module
from xml2aloe import MakeModule

m = Module("binsource")
m.readHeader('/home/ismael/work/osld-lib/scripts/binsource.h')
MakeModule(m)
print m.toString()