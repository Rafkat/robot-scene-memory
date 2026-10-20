//
// Created by Рафкат Галеев on 20/10/2026.
//

#include <stdexcept>

#include <gtest/gtest.h>

#include "robot_scene_sensors/cluster_geometry.hpp"

namespace rsm = robot_scene_sensors;

TEST(ClusterGeometry, Centroid)
{
    const rsm::Cluster cluster{
        {1.0f, 2.0f},
        {3.0f, 4.0f},
        {5.0f, 6.0f}
    };

    const auto result =
      rsm::compute_cluster_geometry(cluster);

    EXPECT_NEAR(result.centroid.x, 3.0f, 1e-5f);
    EXPECT_NEAR(result.centroid.y, 4.0f, 1e-5f);

    EXPECT_EQ(result.point_count, 3u);
}

TEST(ClusterGeometry, BoundingBox)
{
    const rsm::Cluster cluster{
        {-3.0f, 2.0f},
        {1.0f, -4.0f},
        {5.0f, 6.0f}
    };

    const auto result =
      rsm::compute_cluster_geometry(cluster);

    EXPECT_FLOAT_EQ(result.bbox.min_x, -3.0f);
    EXPECT_FLOAT_EQ(result.bbox.max_x, 5.0f);

    EXPECT_FLOAT_EQ(result.bbox.min_y, -4.0f);
    EXPECT_FLOAT_EQ(result.bbox.max_y, 6.0f);
}

TEST(ClusterGeometry, SinglePoint)
{
    const rsm::Cluster cluster{
        {2.5f, -1.0f}
    };

    const auto result =
      rsm::compute_cluster_geometry(cluster);

    EXPECT_EQ(result.point_count, 1u);
    EXPECT_FLOAT_EQ(result.centroid.x, 2.5f);
    EXPECT_FLOAT_EQ(result.centroid.y, -1.0f);

    EXPECT_FLOAT_EQ(result.bbox.min_x, 2.5f);
    EXPECT_FLOAT_EQ(result.bbox.max_x, 2.5f);
}

TEST(ClusterGeometry, EmptyCluster)
{
    const rsm::Cluster cluster{};

    EXPECT_THROW(
      rsm::compute_cluster_geometry(cluster),
      std::invalid_argument
    );
}
