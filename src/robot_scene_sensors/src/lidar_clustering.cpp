//
// Created by rafkat on 10/15/26.
//
#include "robot_scene_sensors/lidar_clustering.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace robot_scene_sensors
{
std::vector<Cluster> cluster_scan(
  const std::vector<float> & ranges,
  const ScanGeometry & geometry,
  float distance_threshold,
  std::size_t min_cluster_size)
{
  if (!std::isfinite(distance_threshold) ||
    distance_threshold <= 0.0f ||
    min_cluster_size == 0)
  {
    throw std::invalid_argument("Invalid clustering parameters");
  }

  std::vector<Cluster> clusters;
  Cluster current_cluster;

  auto finish_cluster = [&]() {
      if (current_cluster.size() >= min_cluster_size) {
        clusters.push_back(std::move(current_cluster));
      }
      current_cluster.clear();
    };

  for (std::size_t i = 0; i < ranges.size(); ++i) {
    const float r = ranges[i];

    if (!std::isfinite(r) ||
      r < geometry.range_min ||
      r > geometry.range_max)
    {
      finish_cluster();
      continue;
    }

    const float angle = geometry.angle_min +
      static_cast<float>(i) * geometry.angle_increment;

    const Point2D point {
      r * std::cos(angle),
      r * std::sin(angle)
    };

    if (!current_cluster.empty()) {
      const auto & previous = current_cluster.back();

      const float distance = std::hypot(
                    point.x - previous.x,
                    point.y - previous.y
      );

      if (distance > distance_threshold) {
        finish_cluster();
      }
    }

    current_cluster.push_back(point);
  }

  finish_cluster();
  return clusters;

}
}
