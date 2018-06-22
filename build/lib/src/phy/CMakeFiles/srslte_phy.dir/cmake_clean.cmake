file(REMOVE_RECURSE
  "libsrslte_phy.pdb"
  "libsrslte_phy.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/srslte_phy.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
