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
