#include <algorithm>
#include <cmath>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

#include "robot_scene_sensors/lidar_geometry.hpp"

using robot_scene_sensors::ScanGeometry;
using robot_scene_sensors::polar_to_cartesian;

class LidarProcessor : public rclcpp::Node {
public:
  LidarProcessor()
  : Node("lidar_processor")
  {

    subscription_ =
      create_subscription<sensor_msgs::msg::LaserScan>(
                "/scan",
                rclcpp::SensorDataQoS(),
      [this](sensor_msgs::msg::LaserScan::ConstSharedPtr msg) {
        process_scan(*msg);
                }
      );

    RCLCPP_INFO(
            get_logger(),
            "Lidar processor started"
    );
  }

private:
  void process_scan(
    const sensor_msgs::msg::LaserScan & scan
  )
  {
    ScanGeometry geometry{
      scan.angle_min,
      scan.angle_increment,
      scan.range_min,
      scan.range_max
    };

    const auto points = polar_to_cartesian(
            scan.ranges,
            geometry
    );

    if (points.empty()) {
      RCLCPP_WARN(
                get_logger(),
                "No valid LiDAR points"
      );
      return;
    }

    const auto closest = std::min_element(
            points.begin(),
            points.end(),
      [](const auto & a, const auto & b) {
        return std::hypot(a.x, a.y) <
               std::hypot(b.x, b.y);
            }
    );

    const float distance = std::hypot(
            closest->x,
            closest->y
    );

    RCLCPP_INFO_THROTTLE(
            get_logger(),
            *get_clock(),
            1000,
            "Valid points: %zu | Closest: %.3f m "
            "| Position: (%.3f, %.3f)",
            points.size(),
            distance,
            closest->x,
            closest->y
    );
  }

  rclcpp::Subscription<
    sensor_msgs::msg::LaserScan
  >::SharedPtr subscription_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<LidarProcessor>();
  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}
