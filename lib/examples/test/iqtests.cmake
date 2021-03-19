#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

message("CMD: " ${CMD})
message("ARG: " ${ARG})
execute_process(COMMAND ${CMD} ${ARG}
           RESULT_VARIABLE result
           OUTPUT_VARIABLE output
           )
message("RESULT: " ${result})
message("OUTPUT: " ${output})
