from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    
    # Robot A检测节点
    detector_node = Node(
        package='robot_coordination',
        executable='detector_node',
        output='screen',
        parameters=[{
            'x': 0.0,
            'y': 0.0,
            'z': 0.0,
            'robot_name': 'robot_a'
        }]
    )
    
    # 指挥中心节点
    center_node = Node(
        package='robot_coordination',
        executable='center_node',
        output='screen',
    )
    
    # Robot B执行节点
    executor_node = Node(
        package='robot_coordination',
        executable='executor_node',
        output='screen',
    )
    
    
    return LaunchDescription([
        detector_node,
        center_node,
        executor_node,
    ])