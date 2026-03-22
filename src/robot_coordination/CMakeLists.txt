cmake_minimum_required(VERSION 3.8)
project(robot_coordination)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(coordination_interfaces REQUIRED)

# 创建可执行文件
add_executable(detector_node src/detector_node.cpp)
ament_target_dependencies(detector_node rclcpp std_msgs geometry_msgs coordination_interfaces)

add_executable(center_node src/center_node.cpp)
ament_target_dependencies(center_node rclcpp std_msgs geometry_msgs tf2 tf2_ros tf2_geometry_msgs coordination_interfaces)

add_executable(executor_node src/executor_node.cpp)
ament_target_dependencies(executor_node rclcpp std_msgs geometry_msgs coordination_interfaces)

# 安装可执行文件
install(TARGETS
  detector_node
  center_node
  executor_node
  DESTINATION lib/${PROJECT_NAME}
)

# 安装launch文件
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()