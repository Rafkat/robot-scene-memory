//
// Created by rafkat on 10/15/26.
//

#ifndef ROBOT_SCENE_MEMORY_LIDAR_CLUSTERING_H
#define ROBOT_SCENE_MEMORY_LIDAR_CLUSTERING_H

#pragma once

#include <vector>

#include "robot_scene_sensors/lidar_geometry.hpp"

namespace robot_scene_sensors
{
using Cluster = std::vector<Point2D>;

std::vector<Cluster> cluster_scan(
  const std::vector<float> & ranges,
  const ScanGeometry & geometry,
  float distance_threshold,
  std::size_t min_cluster_size = 1
);
}


#endif //ROBOT_SCENE_MEMORY_LIDAR_CLUSTERING_H
