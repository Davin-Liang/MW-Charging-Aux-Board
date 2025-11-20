# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\PowerOptimizer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\PowerOptimizer_autogen.dir\\ParseCache.txt"
  "PowerOptimizer_autogen"
  )
endif()
