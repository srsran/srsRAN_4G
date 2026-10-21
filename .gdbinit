# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

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
        start = self.val['buffer']['_M_elems'].cast(self.value_type.pointer())
        length = int(self.val['size_'])
        for idx in range(length):
            yield f'[{idx}]', start[idx]

    def to_string(self):
        length = int(self.val['size_'])
        capacity = int(self.val.type.template_argument(1))
        return f'bounded_vector of length {length}, capacity {capacity}'

    def display_hint(self):
        return 'array'

def make_bounded_vector(val):
    if 'bounded_vector<' in str(val.type):
        return BoundedVectorPrinter(val)

gdb.pretty_printers.append(make_bounded_vector)

end
