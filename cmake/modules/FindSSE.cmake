# Check if SSE instructions are available on the machine where 
# the project is compiled.

# Minimum requirement to enable SSE turbo decoder is SSE4.1
# Since SSE 4.1 includes all previous SSE, look only for this one. 

# Check also AVX availability (for equalizer)

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
   EXEC_PROGRAM(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)

   STRING(REGEX REPLACE "^.*(sse4_1).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "sse4_1" "${SSE_THERE}" SSE41_TRUE)
   IF (SSE41_TRUE)
      set(SSE4_1_FOUND true CACHE BOOL "SSE4.1 available on host")
   ELSE (SSE41_TRUE)
      set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
   ENDIF (SSE41_TRUE)

   STRING(REGEX REPLACE "^.*(sse4_2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "sse4_2" "${SSE_THERE}" SSE42_TRUE)
   IF (SSE42_TRUE)
      set(SSE4_2_FOUND true CACHE BOOL "SSE4.2 available on host")
   ELSE (SSE42_TRUE)
      set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")
   ENDIF (SSE42_TRUE)

   STRING(REGEX REPLACE "^.*(avx).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "avx" "${SSE_THERE}" AVX_TRUE)
   IF (AVX_TRUE)
      set(AVX_FOUND true CACHE BOOL "AVX available on host")
   ELSE (AVX_TRUE)
      set(AVX_FOUND false CACHE BOOL "AVX available on host")
   ENDIF (AVX_TRUE)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Darwin")
   EXEC_PROGRAM("/usr/sbin/sysctl -n machdep.cpu.features" OUTPUT_VARIABLE
      CPUINFO)

   STRING(REGEX REPLACE "^.*(SSE4.1).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSE4.1" "${SSE_THERE}" SSE41_TRUE)
   IF (SSE41_TRUE)
      set(SSE4_1_FOUND true CACHE BOOL "SSE4.1 available on host")
   ELSE (SSE41_TRUE)
      set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
   ENDIF (SSE41_TRUE)

   STRING(REGEX REPLACE "^.*(SSE4.2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSE4.2" "${SSE_THERE}" SSE42_TRUE)
   IF (SSE42_TRUE)
      set(SSE4_2_FOUND true CACHE BOOL "SSE4.2 available on host")
   ELSE (SSE42_TRUE)
      set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")
   ENDIF (SSE42_TRUE)

   STRING(REGEX REPLACE "^.*(AVX).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "AVX" "${SSE_THERE}" AVX_TRUE)
   IF (AVX_TRUE)
      set(AVX_FOUND true CACHE BOOL "AVX available on host")
   ELSE (AVX_TRUE)
      set(AVX_FOUND false CACHE BOOL "AVX available on host")
   ENDIF (AVX_TRUE)
   
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
   # TODO
   set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")
   set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
   set(AVX_FOUND    false CACHE BOOL "AVX    available on host")
ELSE(CMAKE_SYSTEM_NAME MATCHES "Linux")
   set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
   set(AVX_FOUND    false CACHE BOOL "AVX    available on host")
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

if(NOT SSE4_1_FOUND)
      MESSAGE(STATUS "Could not find hardware support for SSE4.1 on this machine.")
endif(NOT SSE4_1_FOUND)

if(NOT SSE4_2_FOUND)
      MESSAGE(STATUS "Could not find hardware support for SSE4.2 on this machine.")
endif(NOT SSE4_2_FOUND)

if(NOT AVX_FOUND)
      MESSAGE(STATUS "Could not find hardware support for AVX on this machine.")
endif(NOT AVX_FOUND)

mark_as_advanced(SSE4_1_FOUND AVX_FOUND)