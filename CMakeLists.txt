cmake_minimum_required(VERSION 3.9)
project(CPlusPlusTest CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.65.0 REQUIRED log_setup log)

include_directories(${Boost_INCLUDE_DIRS})


# logger
add_executable(logger logger.cpp)
target_link_libraries(logger ${Boost_LIBRARIES})

# object_pool
add_executable(object_pool object_pool.cpp)
target_link_libraries(object_pool ${Boost_LIBRARIES})

add_executable(ifstream_with_fd ifstream_with_fd.cpp)

add_executable(string_move_string_view_test string_move_string_view_test.cpp)

add_executable(zlib_toy zlib_toy.cpp)
target_link_libraries(zlib_toy z)

add_executable(iostream_read iostream_read.cpp)

add_executable(counting_bits counting_bits.cpp)


#boost_multimap_test
add_executable(boost_multimap_test boost_multimap_test.cpp)
target_link_libraries(boost_multimap_test ${Boost_LIBRARIES})
