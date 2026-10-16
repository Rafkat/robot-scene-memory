//
// Created by rafkat on 10/15/26.
//

#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include "robot_scene_sensors/lidar_clustering.hpp"

namespace rsm = robot_scene_sensors;

TEST(LidarClustering, SplitByInvalidRange)
{
    const rsm::ScanGeometry geometry{
    0.0f, 0.01f, 0.1f, 10.0f
      };

    const float inf =
    std::numeric_limits<float>::infinity();

    const std::vector<float> ranges{
    2.0f, 2.0f, 2.0f,
    inf,
    3.0f, 3.0f,
    inf,
    4.0f, 4.0f, 4.0f
      };

    const auto clusters =
    rsm::cluster_scan(ranges, geometry, 0.3f);

    ASSERT_EQ(clusters.size(), 3u);

    EXPECT_EQ(clusters[0].size(), 3u);
    EXPECT_EQ(clusters[1].size(), 2u);
    EXPECT_EQ(clusters[2].size(), 3u);
}

TEST(LidarClustering, SplitByDistance)
{
    const rsm::ScanGeometry geometry{
    0.0f, 0.01f, 0.1f, 10.0f
      };

    const std::vector<float> ranges{
    2.0f, 2.0f, 5.0f, 5.0f
      };

    const auto clusters =
    rsm::cluster_scan(ranges, geometry, 0.3f);

    ASSERT_EQ(clusters.size(), 2u);

    EXPECT_EQ(clusters[0].size(), 2u);
    EXPECT_EQ(clusters[1].size(), 2u);
}

TEST(LidarClustering, MinimumClusterSize)
{
    const rsm::ScanGeometry geometry{
    0.0f, 0.01f, 0.1f, 10.0f
      };

    const float inf =
    std::numeric_limits<float>::infinity();

    const auto clusters = rsm::cluster_scan(
    {2.0f, inf, 3.0f, 3.0f},
      geometry,
      0.3f,
      2
    );

    ASSERT_EQ(clusters.size(), 1u);
    EXPECT_EQ(clusters[0].size(), 2u);
}

TEST(LidarClustering, EmptyScan)
{
    const rsm::ScanGeometry geometry{
    0.0f, 0.01f, 0.1f, 10.0f
      };

    const auto clusters =
    rsm::cluster_scan({}, geometry, 0.3f);

    EXPECT_TRUE(clusters.empty());
}


TEST(LidarClustering, AdaptiveThresholdIncreasesWithDistance)
{
  constexpr float angle_increment = 0.0175f;
  constexpr float min_threshold = 0.05f;
  constexpr float scale = 1.5f;

  const float near_threshold =
    rsm::compute_adaptive_threshold(
      2.0f,
      angle_increment,
      min_threshold,
      scale
    );

  const float far_threshold =
    rsm::compute_adaptive_threshold(
      8.0f,
      angle_increment,
      min_threshold,
      scale
    );

  EXPECT_NEAR(near_threshold, 0.1025f, 1e-5f);
  EXPECT_NEAR(far_threshold, 0.26f, 1e-5f);

  EXPECT_GT(far_threshold, near_threshold);
}

TEST(LidarClustering, AdaptiveThresholdKeepsFarPointsTogether)
{
  const rsm::ScanGeometry geometry{
    0.0f,
    0.0175f,
    0.1f,
    10.0f
  };

  const std::vector<float> ranges{
    8.0f, 8.0f, 8.0f
  };

  const auto clusters =
    rsm::cluster_scan(ranges, geometry, 0.05f, 2);

  ASSERT_EQ(clusters.size(), 1u);
  EXPECT_EQ(clusters[0].size(), 3u);
}
