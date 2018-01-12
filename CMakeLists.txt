cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(ibow-lcd LANGUAGES CXX)

# Setting build type to Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Setting the flags for profiling information or not
if(CMAKE_BUILD_TYPE MATCHES Release)
  message(STATUS "Setting Release options")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -march=native")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native")
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
  message(STATUS "Setting Debug options")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O1 -pg -march=native")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O1 -pg -march=native")
endif()

# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   message(STATUS "Using flag -std=c++0x.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.
                         Please use a different C++ compiler.")
endif()

# Printing the compiling flags
message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")

# Catkin dependencies
find_package(catkin REQUIRED COMPONENTS roscpp obindex2)

# Other packages
find_package(OpenCV REQUIRED) # OpenCV
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(OpenMP REQUIRED) # OpenMP
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Defining the package
catkin_package(
    INCLUDE_DIRS include external
    LIBRARIES lcdetector
    CATKIN_DEPENDS roscpp obindex2
    DEPENDS OpenCV Boost
)

include_directories(include
                    external
                    ${catkin_INCLUDE_DIRS}
                    ${OpenCV_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS})

# Copy test images to the binary directory
file(GLOB JSON "evaluation/*.json")
file(COPY ${JSON}
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

### Targets ###
# Library
add_library(lcdetector
            include/ibow-lcd/island.h
            src/lcdetector.cc)
target_link_libraries(lcdetector
                      ${catkin_LIBRARIES}
                      ${OpenCV_LIBRARIES}
                      ${Boost_LIBRARIES})

# Main / Demo
add_executable(demo
               src/main.cc)
target_link_libraries(demo
                      lcdetector
                      ${catkin_LIBRARIES}
                      ${OpenCV_LIBRARIES}
                      ${Boost_LIBRARIES})

# Evaluation
add_executable(evaluator
               evaluation/lcevaluator.cc
               evaluation/main.cc)
target_link_libraries(evaluator
                      lcdetector
                      ${catkin_LIBRARIES}
                      ${OpenCV_LIBRARIES}
                      ${Boost_LIBRARIES})
