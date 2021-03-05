project(tests)
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -march=native -pthread -latomic")
# adding the Google_Tests_run target
add_executable(GenRandArray GenRandArray.cpp)
add_executable(PSort PSortTest.cpp)

# linking Google_Tests_run with DateConverter_lib which will be tested
target_link_libraries(PSort PODArray gtest gtest_main numa spdlog::spdlog)
target_link_libraries(GenRandArray PODArray spdlog::spdlog)

add_executable(ODHT OpenDHTTest.cpp)
target_link_libraries(ODHT PODArray spdlog::spdlog opendht)