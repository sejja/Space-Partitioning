cmake_minimum_required(VERSION 3.8)
project(test)

############################
# Testing
add_executable(${PROJECT_NAME}
			   common.hpp
			   common.cpp
			   test_octree.cpp
			   )
target_link_libraries(${PROJECT_NAME} PUBLIC engine)

# GTest
find_package(GTest CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest GTest::gtest_main) 



