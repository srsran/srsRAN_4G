if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|^i[3,9]86$")
    return()
endif()

include(CheckCSourceRuns)

option(ENABLE_SSE "Enable compile-time SSE4.1 support." ON)
option(ENABLE_AVX "Enable compile-time AVX support."  ON)

if (ENABLE_SSE)
    #
    # Check compiler for SSE4_1 intrinsics
    #
    if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG )
        set(CMAKE_REQUIRED_FLAGS "-msse4.1")
        check_c_source_runs("
        #include <emmintrin.h>
        #include <smmintrin.h>

        int main()
        {
        __m128i a = _mm_setzero_si128();
        __m128i b = _mm_minpos_epu16(a);
        return 0;
        }"
        HAVE_SSE)
    endif()

    if (HAVE_SSE)        
        message(STATUS "SSE4.1 is enabled - target CPU must support it")
    endif()
    
    if (ENABLE_AVX)

        #
        # Check compiler for AVX intrinsics
        #
        if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG )
            set(CMAKE_REQUIRED_FLAGS "-mavx")
            check_c_source_runs("
            #include <immintrin.h>

            int main()
            {
            __m256i a = _mm256_setzero_si256();
            return 0;
            }"
            HAVE_AVX)
        endif()

        if (HAVE_AVX)
            message(STATUS "AVX is enabled - target CPU must support it")
        endif()
    endif()

endif()

mark_as_advanced(HAVE_SSE, HAVE_AVX)