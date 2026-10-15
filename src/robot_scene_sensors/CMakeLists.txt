cmake_minimum_required(VERSION 3.8)
project(robot_scene_sensors)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

add_library(lidar_geometry src/lidar_geometry.cpp)

target_include_directories(lidar_geometry
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

add_executable(
	sensor_node
	src/sensor_node.cpp
)

ament_target_dependencies(
	sensor_node
	rclcpp
)

add_executable(
	lidar_processor src/lidar_processor.cpp
)

ament_target_dependencies(lidar_processor
	rclcpp
	sensor_msgs
)

target_link_libraries(lidar_processor lidar_geometry)

install(
	TARGETS sensor_node lidar_processor
	DESTINATION lib/${PROJECT_NAME}
)

install(
	TARGETS lidar_geometry
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

install(
	DIRECTORY include/
	DESTINATION include
)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
