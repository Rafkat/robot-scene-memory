#include <algorithm>
#include <cmath>
#include <memory>
#include <sstream>
#include <cstdint>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include <visualization_msgs/msg/marker.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>

#include <tf2/exceptions.hpp>
#include <tf2_ros/buffer.hpp>
#include <tf2_ros/transform_listener.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include "../include/robot_scene_sensors/cluster_geometry.hpp"
#include "robot_scene_sensors/lidar_geometry.hpp"
#include "robot_scene_sensors/lidar_clustering.hpp"
#include "robot_scene_sensors/cluster_geometry.hpp"

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

    centroids_publisher_ =
      create_publisher<visualization_msgs::msg::MarkerArray>(
        "/cluster_centroids",
        10
      );

    bbox_publisher_ =
      create_publisher<visualization_msgs::msg::MarkerArray>(
        "/cluster_bboxes",
        10
      );

    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(get_clock());

    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

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

    const auto clusters = cluster_scan(
              scan.ranges,
              geometry,
              0.05f,
              3
    );

    publish_centroids(clusters, scan);
    publish_bboxes(clusters, scan);
    log_centroids_in_base_link(clusters, scan);

    std::ostringstream output;

    output << "Detected clusters: " << clusters.size();

    for (std::size_t i = 0; i < clusters.size(); ++i) {
      const auto & cluster = clusters[i];

      const auto result = compute_cluster_geometry(cluster);

      const float width = result.bbox.max_x - result.bbox.min_x;
      const float height = result.bbox.max_y - result.bbox.min_y;

      output << "\nCluster " << i
             << " | Points: " << result.point_count
             << " | Centroid: ("
             << result.centroid.x << ", "
             << result.centroid.y << ")"
             << " | BBox: "
             << width << " x " << height;
    }

    RCLCPP_INFO_THROTTLE(
      get_logger(),
      *get_clock(),
      1000,
      "%s",
      output.str().c_str()
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

  void publish_centroids(
    const std::vector<robot_scene_sensors::Cluster> & clusters,
    const sensor_msgs::msg::LaserScan & scan
  ) {
    visualization_msgs::msg::MarkerArray marker_array;

    for (std::size_t i = 0; i < clusters.size(); ++i) {
      const auto geometry =
        robot_scene_sensors::compute_cluster_geometry(clusters[i]);

      visualization_msgs::msg::Marker marker;

      marker.header = scan.header;
      marker.ns = "cluster_centroids";
      marker.id = static_cast<int32_t>(i);

      marker.type = visualization_msgs::msg::Marker::SPHERE;
      marker.action = visualization_msgs::msg::Marker::ADD;

      marker.pose.position.x = geometry.centroid.x;
      marker.pose.position.y = geometry.centroid.y;
      marker.pose.position.z = 0.0;

      marker.pose.orientation.w = 1.0;

      marker.scale.x = 0.15;
      marker.scale.y = 0.15;
      marker.scale.z = 0.15;

      marker.color.r = 1.0f;
      marker.color.g = 0.35f;
      marker.color.b = 0.1f;
      marker.color.a = 1.0f;

      marker.lifetime.sec = 0;
      marker.lifetime.nanosec = 500000000;

      marker_array.markers.push_back(marker);
    }

    centroids_publisher_->publish(marker_array);
  }

  void publish_bboxes(
    const std::vector<robot_scene_sensors::Cluster> & clusters,
    const sensor_msgs::msg::LaserScan & scan
  ) {
    visualization_msgs::msg::MarkerArray marker_array;

    constexpr double min_visual_size = 0.03;
    constexpr double marker_depth = 0.05;

    for (std::size_t i = 0; i < clusters.size(); ++i) {
      const auto geometry = robot_scene_sensors::compute_cluster_geometry(clusters[i]);

      const auto & bbox = geometry.bbox;

      const double center_x = (static_cast<double>(bbox.min_x) + bbox.max_x) / 2.0;
      const double center_y = (static_cast<double>(bbox.min_y) + bbox.max_y) / 2.0;

      const double width = bbox.max_x - bbox.min_x;
      const double height = bbox.max_y - bbox.min_y;

      visualization_msgs::msg::Marker marker;

      marker.header = scan.header;
      marker.ns = "cluster_bboxes";
      marker.id = static_cast<int32_t>(i);

      marker.type = visualization_msgs::msg::Marker::CUBE;
      marker.action = visualization_msgs::msg::Marker::ADD;

      marker.pose.position.x = center_x;
      marker.pose.position.y = center_y;
      marker.pose.position.z = 0.0;

      marker.pose.orientation.w = 1.0;

      marker.scale.x = std::max(width, min_visual_size);
      marker.scale.y = std::max(height, min_visual_size);
      marker.scale.z = marker_depth;

      marker.color.r = 0.1f;
      marker.color.g = 0.9f;
      marker.color.b = 0.3f;
      marker.color.a = 0.35f;

      marker.lifetime.sec = 0;
      marker.lifetime.nanosec = 500000000;

      marker_array.markers.push_back(marker);
    }
    bbox_publisher_->publish(marker_array);
  }

  void log_centroids_in_base_link(
    const std::vector<robot_scene_sensors::Cluster> & clusters,
    const sensor_msgs::msg::LaserScan & scan
  ) {
    if (clusters.empty()) {
      return;
    }

    try {
      const auto transform = tf_buffer_->lookupTransform(
        "base_link",
        scan.header.frame_id,
        tf2::TimePointZero
      );

      for (std::size_t i = 0; i < clusters.size(); ++i) {
        const auto geometry = robot_scene_sensors::compute_cluster_geometry();

        geometry_msgs::msg::PointStamped point_lidar;

        point_lidar.header = scan.header;

        point_lidar.point.x = geometry.centroid.x;
        point_lidar.point.y = geometry.centroid.y;
        point_lidar.point.z = 0.0;

        geometry_msgs::msg::PointStamped point_base;

        tf2::doTransform(
          point_lidar,
          point_base,
          transform
        );

        RCLCPP_INFO_THROTTLE(
          get_logger(),
          *get_clock(),
          1000,
          "Cluster %zu in base_link: (%.3f, %.3f, %.3f)",
          i,
          point_base.point.x,
          point_base.point.y,
          point_base.point.z
        );
      }
    }
    catch (const tf2::TransformException & ex) {
      RCLCPP_WARN_THROTTLE(
        get_logger(),
        *get_clock(),
        2000,
        "TF2 transform unavailable: %s",
        ex.what()
      );
    }
  }

  rclcpp::Subscription<
    sensor_msgs::msg::LaserScan
  >::SharedPtr subscription_;

  rclcpp::Publisher<
      visualization_msgs::msg::MarkerArray
    >::SharedPtr centroids_publisher_;

  rclcpp::Publisher<
    visualization_msgs::msg::MarkerArray
  >::SharedPtr bbox_publisher_;

  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<LidarProcessor>();
  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}
