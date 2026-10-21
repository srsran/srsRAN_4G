# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(examples)
add_subdirectory(test)

########################################################################
# Install library headers
########################################################################
INSTALL(  DIRECTORY include/
          DESTINATION "${INCLUDE_DIR}"
          FILES_MATCHING PATTERN "*.h" )