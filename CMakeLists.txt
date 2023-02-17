cmake_minimum_required(VERSION 3.5)
project(ibow-lcd LANGUAGES CXX)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Ament dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(obindex2_lib REQUIRED)
find_package(OpenMP REQUIRED) # OpenMP
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

###########
## Build ##
###########

include_directories(include
                    external)

### Targets ###
# Set library sources
set(${PROJECT_NAME}_sources
	include/ibow-lcd/island.h
	src/lcdetector.cc)

# Main / Demo
add_executable(demo
               src/main.cc
       	       ${${PROJECT_NAME}_sources})
target_link_libraries(demo ${Boost_LIBRARIES})
ament_target_dependencies(demo rclcpp obindex2_lib)

# Evaluation
add_executable(evaluator
               evaluation/lcevaluator.cc
               evaluation/main.cc
	       ${${PROJECT_NAME}_sources})
target_link_libraries(evaluator ${Boost_LIBRARIES})
ament_target_dependencies(evaluator rclcpp obindex2_lib)

install(TARGETS
  demo
  evaluator
  DESTINATION lib/${PROJECT_NAME}/
)

ament_package()
