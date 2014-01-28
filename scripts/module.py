

from xml.dom.minidom import Document,parseString
from pyclibrary import CParser

class Module:
#	name
	nof_inputs=0
	nof_outputs=0
	init_params = []
	input_params = []
	output_params = []

	def __init__(self,name):
		self.name=name
		self.init_func=name+'_initialize'
		self.work_func=name+'_work'
		self.handler=name+'_hl'
		self.handler_instance=name
		self.init_pm_struct=name+'_init'
		self.input_pm_struct=name+'_ctrl_in'
		self.output_pm_struct=name+'_ctrl_out'
		self.directory_name = 'lib_' + name
		
	def set_input(self,nof_inputs, input_type, input_size):
		self.nof_inputs = nof_inputs
		self.input_type = input_type
		self.input_size = input_size

	def set_output(self,nof_outputs, output_type, output_size):
		self.nof_outputs = nof_outputs
		self.output_type = output_type
		self.output_size = output_size

	def add_init_param(self,name,type,default):
		p={'name':name,'variable':self.name+'.init.'+name,'type':type,'default':default}
		self.init_params.append(p)

	def add_input_param(self,name,type,default):
		p={'name':name,'variable':self.name+'.ctrl_in.'+name,'type':type,'default':default}
		self.input_params.append(p)
		
	def add_output_param(self,name,type,default):
		p={'name':name,'variable':self.name+'.ctrl_in.'+name,'type':type,'default':default}
		self.output_params.append(p)

	def toString(self):
		s = 'name: ' + self.name + '\n'
		s = s + 'handler: ' + self.handler + '\n'
		if self.nof_inputs > 0:
			s = s + str(self.nof_inputs) + ' ' + self.input_type + ' inputs of size ' + str(self.output.size) + '\n'
		else:
			s = s + 'no inputs\n'
		if self.nof_outputs > 0:
			s = s + str(self.nof_outputs) + ' ' + self.output_type + ' outputs of size ' + str(self.output_size) + '\n'
		else:
			s = s + 'no outputs\n'
		
		if self.init_params:
			s = s + 'Initialization parameters:\n'
			for p in self.init_params:
				s = s + '  - ' + p['type'] + ' ' + p['name'] + ' = ' + p['default'] + '\n'
		
		if self.input_params:
			s = s + 'Input parameters:\n'
			for p in self.input_params:
				s = s + '  - ' + p['type'] + ' ' + p['name']
				if p['default'] == None:
					s = s + ' (Mandatory)\n'
				else:
					s = s + ' = ' + p['default'] + '\n'
		
		if self.output_params:
			s = s + 'Output parameters:\n'
			for p in self.output_params:
				s = s + '  - ' + p['type'] + ' ' + p['name'] + '\n'
		return s
	
	def toXML(self):
		root = Document()

		module = root.createElement('module')
		root.appendChild(module)

		module.setAttribute("name",self.name)
		module.setAttribute("handler",self.handler)
		module.setAttribute("handler_instance",self.handler_instance)

		# Functions
		functions = root.createElement("functions")
		module.appendChild(functions)
		functions.setAttribute("initialize",self.init_func)
		functions.setAttribute("work",self.work_func)

		# Interfaces
		inputs = root.createElement("inputs")
		module.appendChild(inputs)
		inputs.setAttribute("num",str(self.nof_inputs))
		inputs.setAttribute("type",self.input_type)
		inputs.setAttribute("size",self.input_size)

		outputs = root.createElement("outputs")
		module.appendChild(outputs)
		outputs.setAttribute("num",str(self.nof_outputs))
		outputs.setAttribute("type",self.output_type)
		outputs.setAttribute("size",self.output_size)

		# Init Parameters
		pinit = root.createElement("init_parameters")
		module.appendChild(pinit)
		for p in self.init_params:
			pi = root.createElement("param")
			pinit.appendChild(pi)
			pi.setAttribute("name",p['name'])
			pi.setAttribute("var",p['variable'])
			pi.setAttribute("type",p['type'])
			pi.setAttribute("default",p['default'])

		pinput = root.createElement("input_parameters")
		module.appendChild(pinput)
		for p in self.input_params:
			pi = root.createElement("param")
			pinput.appendChild(pi)
			pi.setAttribute("name",p['name'])
			pi.setAttribute("var",p['variable'])
			pi.setAttribute("type",p['type'])
			pi.setAttribute("default",p['default'])
			
		poutput = root.createElement("output_parameters")
		module.appendChild(poutput)
		for p in self.input_params:
			pi = root.createElement("param")
			pinput.appendChild(pi)
			pi.setAttribute("name",p['name'])
			pi.setAttribute("var",p['variable'])
			pi.setAttribute("type",p['type'])
			

		return root.toprettyxml()
	
	def readXML(self, string):
		root = parseString(string)
		module = root.getElementsByTagName("module").item(0)
		
		self.name = module.getAttribute("name")
		self.handler = module.getAttribute("handler")
		self.handler_instance = module.getAttribute("handler_instance")

		functions = root.getElementsByTagName("functions").item(0)
		self.init_func = functions.getAttribute("initialize")
		self.work_func = functions.getAttribute("work")

		# Interfaces
		inputs = root.getElementsByTagName("inputs").item(0)
		self.nof_inputs = int(inputs.getAttribute("num"))
		self.input_type = inputs.getAttribute("type")
		self.input_size = inputs.getAttribute("size")

		outputs = root.getElementsByTagName("outputs").item(0)
		self.nof_outputs = int(outputs.getAttribute("num"))
		self.output_type = outputs.getAttribute("type")
		self.output_size = outputs.getAttribute("size")

		pinit = root.getElementsByTagName("init_parameters").item(0)
		for p in pinit.getElementsByTagName("params"):
			self.init_params.appendChild({'name':p.getAttribute("name"),\
										'variable':p.getAttribute("variable"), \
										 'type':p.getAttribute("type"),\
										 'default':p.getAttribute("default")})

		pinput = root.getElementsByTagName("input_parameters").item(0)
		for p in pinput.getElementsByTagName("params"):
			self.input_params.appendChild({'name':p.getAttribute("name"),\
										'variable':p.getAttribute("variable"),\
										'type':p.getAttribute("type"),\
										'default':p.getAttribute("default")})
			
		poutput = root.getElementsByTagName("output_parameters").item(0)
		for p in poutput.getElementsByTagName("params"):
			self.output_params.appendChild({'name':p.getAttribute("name"),\
										'variable':p.getAttribute("variable"),\
										'type':p.getAttribute("type")})
	def findMember(self,members, name):
		for m in members:
			if m[0] == name:
				return m
		return None
	
	def findDefault(self, lines, variable):
		for line in lines:
			if variable in line:
				if 'default' in line.lower():
					return str(int(line.split('=')[1].split('*/')[0]))
        		else:
        			return None

	def findSize(self, lines, variable):
		for line in lines:
			if variable in line:
				if 'size' in line.lower():
					return line.split('=')[1].split('*/')[0]
				else:
					return None
    
	def findLinesStruct(self, lines, struct_name):
		slines = []
		state = "nf"
		for line in lines:
			if state == 'nf':
				if 'struct ' + struct_name in line:
					state = "f1"
			elif state == 'f1':
				if '}' in line:
					state = 'f2'
					return slines
				else:
					slines.append(line)

	def findLinesHandler(self,file_name):
		with open(file_name,'r') as f:
			slines = []
			state = "nf"
			for line in reversed(f.readlines()):
				if state == 'nf':
					if self.handler in line and '}' in line:
						state = "f1"
				elif state == 'f1':
					if 'typedef' in line and 'struct' in line:
						state = 'f2'
						return reversed(slines)
					else:
						slines.append(line)
						
	def readHeader(self, file_name):
		
		p = CParser([file_name])
		h =  p.defs['structs'][p.defs['types'][p.defs['types'][self.handler][0]][1]]['members']
		
		input = self.findMember(h,'input')
		if input == None:
			self.nof_inputs = 0
			self.input_type = None
			self.input_size = 0
		else:
			self.nof_inputs = input[1][2][0]
			self.input_type = input[1][0]
			size = self.findSize(self.findLinesHandler(file_name), 'input')
			if size == None:
				size = '2048*20'			
			self.input_size = size
			
		output = self.findMember(h,'output')
		if output == None:
			self.nof_outputs = 0
			self.output_type = None
			self.output_size = 0
		else:
			self.nof_outputs = output[1][2][0]
			self.output_type = output[1][0]
			size = self.findSize(self.findLinesHandler(file_name), 'output')
			if size == None:
				size = '2048*20'			
			self.output_size = size
		
		initpm = p.defs['structs'].get(self.init_pm_struct)
		if (initpm != None):
			for m in initpm['members']:
				default = self.findDefault(self.findLinesStruct(\
											self.findLinesHandler(file_name),\
											self.init_pm_struct), m[0])
				if default == None:
					default = '0'
				self.init_params.append({'name':m[0],'variable':self.name+'.init.'+m[0],\
										'type':m[1][0],'default':default})
				
		
		inputpm = p.defs['structs'].get(self.input_pm_struct)
		if (inputpm != None):
			for m in inputpm['members']:
				default = self.findDefault(self.findLinesStruct(\
											self.findLinesHandler(file_name),\
											self.input_pm_struct), m[0])
				self.input_params.append({'name':m[0],'variable':self.name+'.ctrl_in.'+m[0],\
										'type':m[1][0],'default':default})

		outputpm = p.defs['structs'].get(self.output_pm_struct)
		if (outputpm != None):
			for m in outputpm['members']:
				self.output_params.append({'name':m[0],'variable':self.name+'.ctrl_out.'+m[0],\
										'type':m[1][0]})
		