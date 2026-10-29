from pathlib import Path

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node

def generate_launch_description():
    sim_dir = Path(
        get_package_share_directory('robot_scene_sim')
    )

    world_file = sim_dir / 'worlds' / 'mobile_robot_world.sdf'
    bridge_file = sim_dir / 'config' / 'bridge.yaml'

    gazebo = ExecuteProcess(
        cmd=[
            'gz', 'sim',
            '-s',
            '-r',
            '--headless-rendering',
            '-v', '4',
            str(world_file),
        ],
        output='screen'
    )

    bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        name='ros_gz_bridge',
        output='screen',
        parameters=[
            {'config_file': str(bridge_file)}
        ]
    )

    static_tf = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='lidar_static_tf',
        output='screen',
        arguments=[
            '--x', '0',
            '--y', '0',
            '--z', '0.5',
            '--roll', '0',
            '--pitch', '0',
            '--yaw', '0',
            '--frame-id', 'base_link',
            '--child-frame-id', 'lidar_link'
        ]
    )

    lidar_processor = Node(
        package='robot_scene_sensors',
        executable='lidar_processor',
        name='lidar_processor',
        output='screen',
        parameters=[
            {'use_sim_time': True}
        ]
    )

    return LaunchDescription([
        gazebo,
        bridge,
        static_tf,
        lidar_processor
    ])