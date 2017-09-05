#if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|^i[3,9]86$")
#   return()
#endif()

include(CheckCSourceRuns)

option(ENABLE_SSE "Enable compile-time SSE4.1 support." ON)
option(ENABLE_AVX "Enable compile-time AVX support."  ON)
option(ENABLE_AVX2 "Enable compile-time AVX2 support."  ON)
option(ENABLE_FMA "Enable compile-time FMA support."  ON)

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
              __m256 a, b, c;
              const float src[8] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
              float dst[8];
              a = _mm256_loadu_ps( src );
              b = _mm256_loadu_ps( src );
              c = _mm256_add_ps( a, b );
              _mm256_storeu_ps( dst, c );
              int i = 0;
              for( i = 0; i < 8; i++ ){
                if( ( src[i] + src[i] ) != dst[i] ){
                  return -1;
                }
              }
              return 0;
            }"
            HAVE_AVX)
        endif()

        if (HAVE_AVX)
            message(STATUS "AVX is enabled - target CPU must support it")
        endif()
    endif()
    
    if (ENABLE_AVX2)

      #
      # Check compiler for AVX intrinsics
      #
      if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG )
          set(CMAKE_REQUIRED_FLAGS "-mavx2")
          check_c_source_runs("
          #include <immintrin.h>
          int main()
          {
            __m256i a, b, c;
            const int src[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
            int dst[8];
            a =  _mm256_loadu_si256( (__m256i*)src );
            b =  _mm256_loadu_si256( (__m256i*)src );
            c = _mm256_add_epi32( a, b );
            _mm256_storeu_si256( (__m256i*)dst, c );
            int i = 0;
            for( i = 0; i < 8; i++ ){
              if( ( src[i] + src[i] ) != dst[i] ){
                return -1;
              }
            }
            return 0;
          }"
          HAVE_AVX2)
      endif()

      if (HAVE_AVX2)
          message(STATUS "AVX2 is enabled - target CPU must support it")
      endif()
    endif()

    if (ENABLE_FMA)

        #
        # Check compiler for AVX intrinsics
        #
        if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG )
            set(CMAKE_REQUIRED_FLAGS "-mfma")
            check_c_source_runs("
            #include <immintrin.h>
            int main()
            {
              __m256 a, b, c, r;
              const float src[8] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
              float dst[8];
              a = _mm256_loadu_ps( src );
              b = _mm256_loadu_ps( src );
              c = _mm256_loadu_ps( src );
              r = _mm256_fmadd_ps( a, b, c );
              _mm256_storeu_ps( dst, r );
              int i = 0;
              for( i = 0; i < 8; i++ ){
                if( ( src[i] * src[i] + src[i] ) != dst[i] ){
                  return -1;
                }
              }
              return 0;
            }"
                    HAVE_FMA)
        endif()

        if (HAVE_FMA)
            message(STATUS "FMA is enabled - target CPU must support it")
        endif()
    endif()

endif()

mark_as_advanced(HAVE_SSE, HAVE_AVX, HAVE_AVX2, HAVE_FMA)
