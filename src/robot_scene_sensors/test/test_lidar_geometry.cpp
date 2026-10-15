//
// Created by rafkat on 10/15/26.
//

#include <cmath>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include "robot_scene_sensors/lidar_geometry.hpp"

namespace rsm = robot_scene_sensors;

constexpr float kPi = 3.14159265358979323846f;

// Test 1: A ray pointing along the positive X axis.
TEST(LidarGeometry, ZeroAngle) {
    const rsm::ScanGeometry geometry{
    0.0f,      // angle_min
    0.1f,      // angle_increment
    0.1f,      // range_min
    10.0f      // range_max
    };

    const auto points =
    rsm::polar_to_cartesian({2.5f}, geometry);

    ASSERT_EQ(points.size(), 1u);

    EXPECT_NEAR(points[0].x, 2.5f, 1e-5f);
    EXPECT_NEAR(points[0].y, 0.0f, 1e-5f);
}

// Test 2: A ray pointing along the positive Y axis.
TEST(LidarGeometry, NinetyDegrees) {
    const rsm::ScanGeometry geometry{
    kPi / 2.0f,
    0.1f,
    0.1f,
    10.0f
    };

    const auto points =
    rsm::polar_to_cartesian({3.0f}, geometry);

    ASSERT_EQ(points.size(), 1u);

    EXPECT_NEAR(points[0].x, 0.0f, 1e-5f);
    EXPECT_NEAR(points[0].y, 3.0f, 1e-5f);
}

// Test 3: Invalid measurements must be discarded.
TEST(LidarGeometry, InvalidRanges) {
    const rsm::ScanGeometry geometry{
    0.0f,
    0.1f,
    0.1f,
    10.0f
    };

    const std::vector<float> ranges{
    2.5f,
    std::numeric_limits<float>::infinity(),
    std::numeric_limits<float>::quiet_NaN(),
    0.05f,
    11.0f
    };

    const auto points =
    rsm::polar_to_cartesian(ranges, geometry);

    ASSERT_EQ(points.size(), 1u);

    EXPECT_NEAR(points[0].x, 2.5f, 1e-5f);
    EXPECT_NEAR(points[0].y, 0.0f, 1e-5f);
}

// Test 4: The second ray uses angle_increment.
TEST(LidarGeometry, AngleIncrement) {
    const rsm::ScanGeometry geometry{
    0.0f,
    kPi / 2.0f,
    0.1f,
    10.0f
    };

    const auto points =
    rsm::polar_to_cartesian({2.0f, 3.0f}, geometry);

    ASSERT_EQ(points.size(), 2u);

    EXPECT_NEAR(points[0].x, 2.0f, 1e-5f);
    EXPECT_NEAR(points[0].y, 0.0f, 1e-5f);

    EXPECT_NEAR(points[1].x, 0.0f, 1e-5f);
    EXPECT_NEAR(points[1].y, 3.0f, 1e-5f);
}
