# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

message("CMD: " ${CMD})
message("ARG: " ${ARG})
execute_process(COMMAND ${CMD} ${ARG}
           RESULT_VARIABLE result
           OUTPUT_VARIABLE output
           )
message("RESULT: " ${result})
message("OUTPUT: " ${output})
