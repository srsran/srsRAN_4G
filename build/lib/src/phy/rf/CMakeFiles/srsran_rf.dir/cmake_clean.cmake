file(REMOVE_RECURSE
  "libsrsran_rf.pdb"
  "libsrsran_rf.so"
  "libsrsran_rf.so.0"
  "libsrsran_rf.so.22.04.1"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/srsran_rf.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
