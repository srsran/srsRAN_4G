# -*- coding: utf-8 -*-
"""
CLibrary.py -  Provides CLibrary class
Copyright 2010  Luke Campagnola
Distributed under MIT/X11 license. See license.txt for more infomation.

Proxy to both CHeader and ctypes, allowing automatic type conversion and
function calling based on C header definitions.
"""


from ctypes import *
import sys


class CLibrary:
    """The CLibrary class is intended to automate much of the work in using ctypes by integrating
    header file definitions from CParser. Ths class serves as a proxy to a ctypes, adding
    a few features:
      - allows easy access to values defined via CParser
      - automatic type conversions for function calls using CParser function signatures
      - creates ctype classes based on type definitions from CParser
      
    Initialize using a ctypes shared object and a CParser:
       headers = CParser.winDefs()
       lib = CLibrary(windll.User32, headers)
      
    There are 3 ways to access library elements:
        lib(type, name) - type can be one of 'values', 'functions', 'types', 'structs', 'unions', or 'enums'.
                          Returns an object matching name. For values, the value from the headers is
                          returned. For functions, a callable object is returned that handles automatic
                          type conversion for arguments and return values. for structs, types, and enums, 
                          a ctypes class is returned matching the type specified.
        lib.name   - searches in order through values, functions, types, structs, unions, and enums from 
                     header definitions and returns an object for the first match found. The object
                     returned is the same as returned by lib(type, name). This is the preferred way to access
                     elements from CLibrary, but may not work in some situations (for example, if
                     a struct and variable share the same name).
        lib[type]  - Accesses the header definitions directly, returns definition dictionaries
                     based on the type requested. This is equivalent to headers.defs[type].
    """
    Null = object()
    
    cTypes = {
        'char': c_char,
        'wchar': c_wchar,
        'unsigned char': c_ubyte,
        'short': c_short,
        'short int': c_short,
        'unsigned short': c_ushort,
        'unsigned short int': c_ushort,
        'int': c_int,
        'unsigned': c_uint,
        'unsigned int': c_uint,
        'long': c_long,
        'long int': c_long,
        'unsigned long': c_ulong,
        'unsigned long int': c_ulong,
        '__int64': c_longlong,
        'long long': c_longlong,
        'long long int': c_longlong,
        'unsigned __int64': c_ulonglong,
        'unsigned long long': c_ulonglong,
        'unsigned long long int': c_ulonglong,
        'float': c_float,
        'double': c_double,
        'long double': c_longdouble
    }
    cPtrTypes = {
        'char': c_char_p,
        'wchar': c_wchar_p,
        'void': c_void_p
    }
        
        
    
    def __init__(self, lib, headers, prefix=None):
        ## name everything using underscores to avoid name collisions with library
        
        self._lib_ = lib
        self._headers_ = headers
        self._defs_ = headers.defs
        if prefix is None:
            self._prefix_ = []
        elif type(prefix) is list:
            self._prefix_ = prefix
        else:
            self._prefix_ = [prefix]
        self._objs_ = {}
        for k in ['values', 'functions', 'types', 'structs', 'unions', 'enums']:
            self._objs_[k] = {}
        self._allObjs_ = {}
        self._structs_ = {}
        self._unions_ = {}

    def __call__(self, typ, name):
        if typ not in self._objs_:
            typs = self._objs_.keys()
            raise Exception("Type must be one of %s" % str(typs))
        
        if name not in self._objs_[typ]:
            self._objs_[typ][name] = self._mkObj_(typ, name)
            
        return self._objs_[typ][name]

    def _allNames_(self, name):
        return [name] + [p + name for p in self._prefix_]

    def _mkObj_(self, typ, name):
        names = self._allNames_(name)
        
        for n in names:
            if n in self._objs_:
                return self._objs_[n]
            
        for n in names:  ## try with and without prefix
            if n not in self._defs_[typ] and not (typ in ['structs', 'unions', 'enums'] and n in self._defs_['types']):
                continue
                
            if typ == 'values':
                return self._defs_[typ][n]
            elif typ == 'functions':
                return self._getFunction(n)
            elif typ == 'types':
                obj = self._defs_[typ][n]
                return self._ctype(obj)
            elif typ == 'structs':
                return self._cstruct('structs', n)
            elif typ == 'unions':
                return self._cstruct('unions', n)
            elif typ == 'enums':
                ## Allow automatic resolving of typedefs that alias enums
                if n not in self._defs_['enums']:
                    if n not in self._defs_['types']:
                        raise Exception('No enums named "%s"' % n)
                    typ = self._headers_.evalType([n])[0]
                    if typ[:5] != 'enum ':
                        raise Exception('No enums named "%s"' % n)
                    n = self._defs_['types'][typ][1]  ## look up internal name of enum 
                obj = self._defs_['enums'][n]
                    
                return obj
            else:
                raise Exception("Unknown type %s" % typ)
        raise NameError(name)
        

    def __getattr__(self, name):
        """Used to retrieve any type of definition from the headers. Searches for the name in this order:
        values, functions, types, structs, unions, enums."""
        if name not in self._allObjs_:
            names = self._allNames_(name)
            for k in ['values', 'functions', 'types', 'structs', 'unions', 'enums', None]:
                if k is None:
                    raise NameError(name)
                obj = None
                for n in names:
                    if n in self._defs_[k]:
                        obj = self(k, n)
                        break
                if obj is not None:
                    break
            self._allObjs_[name] = obj
        return self._allObjs_[name]

    def __getitem__(self, name):
        """Used to retrieve a specific dictionary from the headers."""
        return self._defs_[name]
        
    def __repr__(self):
        return "<CLibrary instance: %s>" % str(self._lib_)
        
    def _getFunction(self, funcName):
        try:
            func = getattr(self._lib_, funcName)
        except:
            raise Exception("Function name '%s' appears in headers but not in library!" % func)
            
        #print "create function %s," % (funcName), self._defs_['functions'][funcName]
        return CFunction(self, func, self._defs_['functions'][funcName], funcName)
        
    def _ctype(self, typ, pointers=True):
        """return a ctype object representing the named type. 
        If pointers is True, the class returned includes all pointer/array specs provided. 
        Otherwise, the class returned is just the base type with no pointers."""
        try:
            typ = self._headers_.evalType(typ)
            mods = typ[1:][:]
            
            ## Create the initial type
            ## Some types like ['char', '*'] have a specific ctype (c_char_p)
            ## (but only do this if pointers == True)
            if pointers and len(typ) > 1 and typ[1] == '*' and typ[0] in CLibrary.cPtrTypes:
                cls = CLibrary.cPtrTypes[typ[0]]
                mods = typ[2:]
                
            ## If the base type is in the list of existing ctypes:
            elif typ[0] in CLibrary.cTypes:
                cls = CLibrary.cTypes[typ[0]]
                
            ## structs, unions, enums:
            elif typ[0][:7] == 'struct ':
                cls = self._cstruct('structs', self._defs_['types'][typ[0]][1])
            elif typ[0][:6] == 'union ':
                cls = self._cstruct('unions', self._defs_['types'][typ[0]][1])
            elif typ[0][:5] == 'enum ':
                cls = c_int
                
            ## void
            elif typ[0] == 'void':
                cls = None
            else:
                #print typ
                raise Exception("Can't find base type for %s" % str(typ))
            
            if not pointers:
                return cls
                
            ## apply pointers and arrays
            while len(mods) > 0:
                m = mods.pop(0)
                if isinstance(m, basestring):  ## pointer or reference
                    if m[0] == '*' or m[0] == '&':
                        for i in m:
                            cls = POINTER(cls)
                elif type(m) is list:          ## array
                    for i in m:
                        if i == -1:            ## -1 indicates an 'incomplete type' like "int variable[]"
                            cls = POINTER(cls) ## which we should interpret like "int *variable"
                        else:
                            cls = cls * i
                elif type(m) is tuple:   ## Probably a function pointer
                    ## Find pointer and calling convention
                    isPtr = False
                    conv = '__cdecl'
                    if len(mods) == 0:
                        raise Exception("Function signature with no pointer:", m, mods)
                    for i in [0,1]:
                        if len(mods) < 1:
                            break
                        if mods[0] == '*':
                            mods.pop(0)
                            isPtr = True
                        elif mods[0] in ['__stdcall', '__cdecl']:
                            conv = mods.pop(0)
                        else:
                            break
                    if not isPtr:
                        raise Exception("Not sure how to handle type (function without single pointer): %s" % str(typ))
                            
                    if conv == '__stdcall':
                        mkfn = WINFUNCTYPE
                    else:
                        mkfn = CFUNCTYPE
                    #print "Create function pointer (%s)" % conv
                    
                    args = [self._ctype(arg[1]) for arg in m]
                    cls = mkfn(cls, *args)
                            
                else:
                    raise Exception("Not sure what to do with this type modifier: '%s'" % str(p))
            return cls
        except:
            print "Error while processing type", typ
            raise
        
    def _cstruct(self, strType, strName):
        if strName not in self._structs_:
            
            ## Resolve struct name--typedef aliases allowed.
            if strName not in self._defs_[strType]:
                if strName not in self._defs_['types']:
                    raise Exception('No struct/union named "%s"' % strName)
                typ = self._headers_.evalType([strName])[0]
                if typ[:7] != 'struct ' and typ[:6] != 'union ':
                    raise Exception('No struct/union named "%s"' % strName)
                strName = self._defs_['types'][typ][1]
                
            ## Pull struct definition
            defn = self._defs_[strType][strName]
                
                
            ## create ctypes class
            defs = defn['members'][:]
            if strType == 'structs':
                class s(Structure):
                    def __repr__(self):
                        return "<ctypes struct '%s'>" % strName
            elif strType == 'unions':
                class s(Union):
                    def __repr__(self):
                        return "<ctypes union '%s'>" % strName
            
            
            ## must register struct here to allow recursive definitions.
            self._structs_[strName] = s
            
            if defn['pack'] is not None:
                s._pack_ = defn['pack']
            
            ## assign names to anonymous members
            members = []
            anon = []
            for i in range(len(defs)):
                if defs[i][0] is None:
                    c = 0
                    while True:
                        name = 'anon_member%d' % c
                        if name not in members:
                            defs[i][0] = name
                            anon.append(name)
                            break
                members.append(defs[i][0])
            
            s._anonymous_ = anon
            s._fields_ = [(m[0], self._ctype(m[1])) for m in defs]
            s._defaults_ = [m[2] for m in defs]
        return self._structs_[strName]
        


class CFunction:
    def __init__(self, lib, func, sig, name):
        self.lib = lib
        self.func = func
        #print sig
        self.sig = list(sig) # looks like [return_type, [(argName, type, default), (argName, type, default), ...]]
        self.sig[1] = [s for s in sig[1] if s[1] != ['void']]  ## remove void args from list
        for conv in ['__stdcall', '__cdecl']:
            if conv in self.sig[0]:
                self.sig[0].remove(conv)
        self.name = name
        self.restype = lib._ctype(self.sig[0])
        #func.restype = self.restype
        self.argTypes = [lib._ctype(s[1]) for s in self.sig[1]]
        func.argtypes = self.argTypes
        self.reqArgs = [x[0] for x in self.sig[1] if x[2] is None]
        self.argInds = dict([(self.sig[1][i][0], i) for i in range(len(self.sig[1]))])  ## mapping from argument names to indices
        #print "created func", self, sig, self.argTypes

    def argCType(self, arg):
        """Return the ctype required for the specified argument.
        arg can be either an integer  or the name of the argument.
        """
        if isinstance(arg, basestring):
            arg = self.argInds[arg]
        return self.lib._ctype(self.sig[1][arg][1])
    
    def __call__(self, *args, **kwargs):
        """Invoke the SO or dll function referenced, converting all arguments to the correct type.
        Keyword arguments are allowed as long as the header specifies the argument names.
        Arguments which are passed byref may be omitted entirely, and will be automaticaly generated.
        To pass a NULL pointer, give None as the argument.
        Returns the return value of the function call as well as all of the arguments (so that objects passed by reference can be retrieved)"""
        #print "CALL: %s(%s)" % (self.name, ", ".join(map(str, args) + ["%s=%s" % (k, str(kwargs[k])) for k in kwargs]))
        #print "  sig:", self.sig
        argList = [None] * max(len(self.reqArgs), len(args))  ## We'll need at least this many arguments.
        
        ## First fill in args
        for i in range(len(args)):
            #argList[i] = self.argTypes[i](args[i])
            if args[i] is None:
                argList[i] = self.lib.Null
            else:
                argList[i] = args[i]
        
        ## Next fill in kwargs
        for k in kwargs:
            #print "    kw:", k
            if k not in self.argInds:
                print "Function signature:", self.prettySignature()
                raise Exception("Function signature has no argument named '%s'" % k)
            ind = self.argInds[k]
            if ind >= len(argList):  ## stretch argument list if needed
                argList += [None] * (ind - len(argList) + 1)
            #argList[ind] = self.coerce(kwargs[k], self.argTypes[ind])
            if kwargs[k] is None:
                argList[ind] = self.lib.Null
            else:
                argList[ind] = kwargs[k]
        
        guessedArgs = []
        ## Finally, fill in remaining arguments if they are pointers to int/float/void*/struct values 
        ## (we assume these are to be modified by the function and their initial value is not important)
        for i in range(len(argList)):
            if argList[i] is None or argList[i] is self.lib.Null:
                try:
                    sig = self.sig[1][i][1]
                    argType = self.lib._headers_.evalType(sig)
                    if argList[i] is self.lib.Null:  ## request to build a null pointer
                        if len(argType) < 2:
                            raise Exception("Can not create NULL for non-pointer argument type: %s" % str(argType))
                        argList[i] = self.lib._ctype(sig)()
                    #elif argType == ['char', '*']:  ## pass null pointer if none was specified. This is a little dangerous, but some functions will expect it.
                        #argList[i] = c_char_p()     ##  On second thought: let's just require the user to explicitly ask for a NULL pointer.
                    else:
                        if argType == ['void', '**'] or argType == ['void', '*', '*']:
                            cls = c_void_p
                        else:
                            assert len(argType) == 2 and argType[1] == '*' ## Must be 2-part type, second part must be '*'
                            cls = self.lib._ctype(sig, pointers=False)
                        argList[i] = pointer(cls(0))
                        guessedArgs.append(i)
                except:
                    if sys.exc_info()[0] is not AssertionError:
                        raise
                        #sys.excepthook(*sys.exc_info())
                    print "Function signature:", self.prettySignature()
                    raise Exception("Function call '%s' missing required argument %d '%s'. (See above for signature)" % (self.name, i, self.sig[1][i][0]))
        #print "  args:", argList
        try:
            res = self.func(*argList)
        except:
            print "Function call failed. Signature is:", self.prettySignature()
            print "Arguments:", argList
            print "Argtypes:", self.func.argtypes
            raise
        #print "  result:", res
        
        cr = CallResult(res, argList, self.sig, guessed=guessedArgs)
        return cr
    
    def prettySignature(self):
        return "%s %s(%s)" % (''.join(self.sig[0]), self.name, ', '.join(["%s %s" % ("".join(map(str, s[1])), s[0]) for s in self.sig[1]]))
    
class CallResult:
    """Class for bundling results from C function calls. Allows access to the function
    return value as well as all of the arguments, since the function call will often return
    extra values via these arguments. 
      - Original ctype objects can be accessed via result.rval or result.args
      - Python values carried by these objects can be accessed using ()
    To access values:
       - The return value: ()
       - The nth argument passed: [n]
       - The argument by name: ['name']
       - All values that were auto-generated: .auto()
       
    The class can also be used as an iterator, so that tuple unpacking is possible:
       ret, arg1, arg2 = lib.runSomeFunction(...)
       """
    def __init__(self, rval, args, sig, guessed):
        self.rval = rval        ## return value of function call
        self.args = args        ## list of arguments to function call
        self.sig = sig          ## function signature
        self.guessed = guessed  ## list of arguments that were generated automatically (usually byrefs)
        
    def __call__(self):
        #print "Clibrary:", type(self.rval), self.mkVal(self.rval)
        if self.sig[0] == ['void']:
            return None
        return self.mkVal(self.rval)
        
    def __getitem__(self, n):
        if type(n) is int:
            return self.mkVal(self.args[n])
        elif type(n) is str:
            ind = self.findArg(n)
            return self.mkVal(self.args[ind])
        else:
            raise Exception("Index must be int or str.")

    def __setitem__(self, n, val):
        if type(n) is int:
            self.args[n] = val
        elif type(n) is str:
            ind = self.findArg(n)
            self.args[ind] = val
        else:
            raise Exception("Index must be int or str.")
        

    def mkVal(self, obj):
        while not hasattr(obj, 'value'):
            if not hasattr(obj, 'contents'):
                return obj
            try:
                obj = obj.contents
            except ValueError:
                return None
        
        return obj.value
        
        
    def findArg(self, arg):
        for i in range(len(self.sig[1])):
            if self.sig[1][i][0] == arg:
                return i
        raise Exception("Can't find argument '%s' in function signature. Arguments are: %s" % (arg, str([a[0] for a in self.sig[1]])))
    
    def __iter__(self):
        yield self()
        for i in range(len(self.args)):
            yield(self[i])
        
    def auto(self):
        return [self[n] for n in self.guessed]
            
    
    
    

