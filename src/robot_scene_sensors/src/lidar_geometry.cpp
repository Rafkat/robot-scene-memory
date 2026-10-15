#include "robot_scene_sensors/lidar_geometry.hpp"

#include <cmath>

namespace robot_scene_sensors
{

std::vector<Point2D> polar_to_cartesian(
  const std::vector<float> & ranges,
  const ScanGeometry & geometry
)
{
  std::vector<Point2D> points;
  points.reserve(ranges.size());

  for (std::size_t i = 0; i < ranges.size(); ++i) {
    const float distance = ranges[i];

    if (!std::isfinite(distance) ||
      distance < geometry.range_min ||
      distance > geometry.range_max)
    {
      continue;
    }

    const float angle =
      geometry.angle_min +
      static_cast<float>(i) * geometry.angle_increment;

    points.push_back({
        distance * std::cos(angle),
        distance * std::sin(angle)
        });
  }

  return points;
}

}  // namespace robot_scene_sensors
