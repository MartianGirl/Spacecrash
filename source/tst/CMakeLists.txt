cmake_minimum_required(VERSION 2.6)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# Add the test executable
include_directories(${PROJECT_SOURCE_DIR}/src ${Boost_INCLUDE_DIR})

add_executable(SpacecrashTest
    SpacecrashTest.cc
    ${PROJECT_SOURCE_DIR}/src/Spacecrash.cc)

target_link_libraries(SpacecrashTest ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
