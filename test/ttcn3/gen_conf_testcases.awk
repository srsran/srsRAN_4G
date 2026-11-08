# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

# Removes the inter-RAT test cases from LTE_EPS_TS_Testcases.ttcn of the 3GPP test suite, as only the E-UTRA components
# are built, and adds a control part executing all remaining test cases.
# Usage: awk -f gen_conf_testcases.awk LTE_EPS_TS_Testcases.ttcn > srsRAN/LTE_EPS_TS_Testcases.ttcn

BEGIN {
  in_testcase = 0
  skip_blank = 0
  nof_testcases = 0
}

{
  sub(/\r$/, "")
}

# The components of the other RATs are replaced by dummy definitions
/^[ \t]*import from .*(CDMA2000|GERAN|UTRAN|InterRat|C2K)/ {
  if ($0 ~ /import from UTRAN_Component /) {
    print "  import from DummyRats all;"
  }
  next
}

skip_blank && /^[ \t]*$/ {
  skip_blank = 0
  next
}

{
  skip_blank = 0
}

!in_testcase && /^[ \t]*testcase[ \t]+/ {
  in_testcase = 1
  depth = 0
  opened = 0
  block = ""
  name = $0
  sub(/^[ \t]*testcase[ \t]+/, "", name)
  sub(/[ \t]*\(.*/, "", name)
}

in_testcase {
  block = block $0 "\n"
  line = $0
  sub(/\/\/.*/, "", line)
  depth += gsub(/{/, "{", line) - gsub(/}/, "}", line)
  if (index(line, "{")) {
    opened = 1
  }
  if (opened && depth == 0) {
    in_testcase = 0
    if (block ~ /(UTRAN|GERAN|CDMA2000)_PTC\.create/) {
      skip_blank = 1
    } else {
      printf "%s", block
      testcases[nof_testcases++] = name
    }
  }
  next
}

# Closing brace of the module
/^}/ {
  print "  control {"
  for (i = 0; i < nof_testcases; i++) {
    print "    execute( " testcases[i] "() );"
  }
  print "  }"
  print ""
}

{
  print
}
