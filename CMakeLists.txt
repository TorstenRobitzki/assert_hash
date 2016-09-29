cmake_minimum_required(VERSION 2.8.7)

include_directories(${CMAKE_SOURCE_DIR})

#set(Boost_DEBUG 1)
find_package( Boost REQUIRED COMPONENTS program_options filesystem system )
if (Boost_FOUND)
    include_directories(BEFORE ${Boost_INCLUDE_DIR})
endif()

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic -Wextra -Wfatal-errors -Wno-unused-parameter -Wno-sign-compare")

add_executable(tests tests.cpp)
add_custom_target(
    tests.run
    COMMAND tests)

add_executable(assert_hashes assert_hashes.cpp)
target_link_libraries(assert_hashes ${Boost_LIBRARIES})
