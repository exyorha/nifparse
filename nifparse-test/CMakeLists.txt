add_executable(nifparse-test
  main.cpp
)

target_link_libraries(nifparse-test PRIVATE nifparse)
set_target_properties(nifparse-test PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
