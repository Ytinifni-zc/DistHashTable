project(tests)
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -march=native -pthread -latomic")
# adding the Google_Tests_run target
add_executable(GenRandArray GenRandArray.cpp)
add_executable(PSort PSortTest.cpp)

# linking Google_Tests_run with DateConverter_lib which will be tested
target_link_libraries(PSort gtest gtest_main numa spdlog::spdlog)
target_link_libraries(GenRandArray spdlog::spdlog)

add_executable(ODHT OpenDHTTest.cpp)
target_link_libraries(ODHT spdlog::spdlog opendht)
add_executable(ODHT2 OpenDHTTest2.cpp)
target_link_libraries(ODHT2 spdlog::spdlog opendht gtest gtest_main)
