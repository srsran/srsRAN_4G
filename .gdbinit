#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

############################################
#            Pretty-Printers
############################################

python

###### srsran::bounded_vector<T, N> ########

class BoundedVectorPrinter(object):
    def __init__(self, val):
        self.val = val
        self.value_type = self.val.type.template_argument(0)

    def children(self):
        start = self.val['buffer'].cast(self.value_type.pointer())
        length = int(self.val['size_'])
        for idx in range(length):
            yield f'[{idx}]', start[idx]

    def to_string(self):
        length = int(self.val['size_'])
        capacity = int(self.val.type.template_argument(1))
        return f'bounded_vector of length {length}, capacity {capacity}'

    def display_hint(self):
        return 'array'

    @staticmethod
    def make(val):
        if str(val.type).startswith('srsran::bounded_vector<'):
            return BoundedVectorPrinter(val)

gdb.pretty_printers.append(BoundedVectorPrinter.make)

end
