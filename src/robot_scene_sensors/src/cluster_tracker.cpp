//
// Created by Рафкат Галеев on 30/10/2026.
//
#include "robot_scene_sensors/cluster_tracker.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace robot_scene_sensors
{
ClusterTracker::ClusterTracker(
  float max_match_distance,
  std::size_t max_missed_frames
)
: max_match_distance_(max_match_distance),
  max_missed_frames_(max_missed_frames)
{
}

void ClusterTracker::update(
  const std::vector<Point2D> & observations
)
{
  const std::size_t old_track_count = tracks_.size();

  std::vector<bool> matched_tracks(old_track_count, false);
  std::vector<bool> matched_observations(observations.size(), false);

  for (std::size_t j = 0; j < observations.size(); ++j) {
    float best_distance = max_match_distance_;
    std::size_t best_index = old_track_count;

    for (std::size_t i = 0; i < old_track_count; ++i) {
      if (matched_tracks[i]) {
        continue;
      }

      const float dx = tracks_[i].position.x - observations[j].x;
      const float dy = tracks_[i].position.y - observations[j].y;

      const float distance = std::hypot(dx, dy);

      if (distance < best_distance) {
        best_distance = distance;
        best_index = i;
      }
    }

    if (best_index != old_track_count) {
      auto & track = tracks_[best_index];

      track.position = observations[j];
      ++track.observations;
      track.missed_frames = 0;

      matched_tracks[best_index] = true;
      matched_observations[j] = true;
    }
  }

  for (std::size_t i = 0; i < old_track_count; ++i) {
    if (!matched_tracks[i]) {
      ++tracks_[i].missed_frames;
    }
  }

  for (std::size_t j = 0; j < observations.size(); ++j) {
    if (!matched_observations[j]) {
      tracks_.push_back({
          next_id_++,
          observations[j],
          1,
          0
                });
    }
  }

  tracks_.erase(
            std::remove_if(
            tracks_.begin(),
            tracks_.end(),
      [this](const TrackedCluster & track) {
        return track.missed_frames > max_missed_frames_;
            }),
          tracks_.end());
}


const std::vector<TrackedCluster> & ClusterTracker::tracks() const
{
  return tracks_;
}

}
