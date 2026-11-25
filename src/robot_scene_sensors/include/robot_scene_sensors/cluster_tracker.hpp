#pragma once

#include <cstddef>
#include <vector>

#include "robot_scene_sensors/lidar_geometry.hpp"

namespace robot_scene_sensors
{

struct TrackedCluster
{
  std::size_t id;
  Point2D position;
  std::size_t observations;
  std::size_t missed_frames;
};

class ClusterTracker {
public:
  explicit ClusterTracker(
    float max_match_distance = 0.5f,
    std::size_t max_missed_frames = 5);

  void update(const std::vector<Point2D> & observations);

  const std::vector<TrackedCluster> & tracks() const;

private:
  float max_match_distance_;
  std::size_t max_missed_frames_;
  std::size_t next_id_ = 0;

  std::vector<TrackedCluster> tracks_;
};
}
