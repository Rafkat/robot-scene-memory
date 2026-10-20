//
// Created by rafkat on 10/15/26.
//
#include "robot_scene_sensors/lidar_clustering.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>
#include <algorithm>

namespace robot_scene_sensors
{
float compute_adaptive_threshold(
  float range,
  float angle_increment,
  float min_threshold,
  float scale
)
{
  return min_threshold + scale * range * std::abs(angle_increment);
}

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

  float previous_range = 0.0f;
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

      const float adaptive_threshold = compute_adaptive_threshold(
        std::max(previous_range, r),
        geometry.angle_increment,
        distance_threshold,
        1.5f
      );

      if (distance > adaptive_threshold) {
        finish_cluster();
      }
    }

    current_cluster.push_back(point);
    previous_range = r;
  }

  finish_cluster();
  return clusters;

}
}
