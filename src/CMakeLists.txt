add_definitions(-std=c++17)

set(CXX_FLAGS
  "-Wall"
  "-Wno-c++98-compat"
  "-Wno-deprecated-declarations"
  "-Wno-padded"
)

set(CXX_TEST_FLAGS
  ${CXX_FLAGS}
  "-Wno-global-constructors"
)

add_executable(main
  main.cc
)
target_compile_options(main PRIVATE ${CXX_FLAGS})
