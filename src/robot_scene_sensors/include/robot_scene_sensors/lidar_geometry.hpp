#ifndef ROBOT_SCENE_MEMORY_LIDAR_GEOMETRY_H
#define ROBOT_SCENE_MEMORY_LIDAR_GEOMETRY_H

#pragma once

#include <vector>

namespace robot_scene_sensors
{

struct Point2D
{
  float x;
  float y;
};

struct ScanGeometry
{
  float angle_min;
  float angle_increment;
  float range_min;
  float range_max;
};

std::vector<Point2D> polar_to_cartesian(
  const std::vector<float> & ranges,
  const ScanGeometry & geometry
);

}  // namespace robot_scene_sensors
#endif //ROBOT_SCENE_MEMORY_LIDAR_GEOMETRY_H
