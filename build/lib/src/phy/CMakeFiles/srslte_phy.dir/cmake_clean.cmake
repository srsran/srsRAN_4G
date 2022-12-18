file(REMOVE_RECURSE
  "libsrslte_phy.a"
  "libsrslte_phy.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang C CXX)
  include(CMakeFiles/srslte_phy.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
