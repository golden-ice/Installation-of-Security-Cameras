cmake_minimum_required(VERSION 2.8...3.13)
project(streets_and_shortest_paths)

## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/run)

option(WITH_ASAN "Enabled AddressSanitizer to detect memory faults" OFF)
if(WITH_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined" )
endif()


# create the main executable
## add additional .cpp files if needed
add_executable(find-shortest-paths find-shortest-paths.cpp )
add_executable(rgen rgen.cpp )
add_executable(streets_and_shortest_paths streets_and_shortest_paths.cpp )

# create the executable for tests
# add_executable(tests test.cpp)
install(PROGRAMS intersection.py streets.py DESTINATION bin)
install(TARGETS find-shortest-paths rgen streets_and_shortest_paths RUNTIME DESTINATION bin)
