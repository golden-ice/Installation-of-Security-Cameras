cmake_minimum_required(VERSION 2.8.12)
project(algo-compare)

if (EXISTS "${CMAKE_SOURCE_DIR}/minisat")
  add_subdirectory(minisat)
else()
  message (FATAL_ERROR "MiniSat not found. Use `git clone https://git.uwaterloo.ca/ece650-f23/minisat.git` to check it out.")
endif()


## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")

# expose minisat header files
include_directories(${CMAKE_SOURCE_DIR}/minisat)

option(WITH_ASAN "Enabled AddressSanitizer to detect memory faults" OFF)
if(WITH_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined" )
endif()

# linke the pthread package
find_package (Threads)

# create the main executable
## add additional .cpp files if needed
add_executable(algo-compare algo-compare.cpp)
# add_executable(alogo-compare-minisat alogo-compare-minisat.cpp)
# link MiniSAT libraries
target_link_libraries(algo-compare minisat-lib-static)
# link pthread libraries
target_link_libraries(algo-compare ${CMAKE_THREAD_LIBS_INIT})

# create the executable for tests
# add_executable(test test.cpp)
# link MiniSAT libraries
# target_link_libraries(test minisat-lib-static)
