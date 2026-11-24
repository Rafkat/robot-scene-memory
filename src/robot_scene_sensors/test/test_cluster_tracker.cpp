//
// Created by Рафкат Галеев on 30/10/2026.
//

#include <gtest/gtest.h>

#include "robot_scene_sensors/cluster_tracker.hpp"

using robot_scene_sensors::ClusterTracker;
using robot_scene_sensors::Point2D;

TEST(ClusterTrackerTest, CreatesTracks)
{
    ClusterTracker tracker;

    tracker.update({
    Point2D{2.5f, 0.0f},
    Point2D{0.0f, 2.5f}
    });

    ASSERT_EQ(tracker.tracks().size(), 2u);
    EXPECT_EQ(tracker.tracks()[0].id, 0u);
    EXPECT_EQ(tracker.tracks()[1].id, 1u);
}

TEST(ClusterTrackerTest, PreservesIds)
{
    ClusterTracker tracker;

    tracker.update({
    Point2D{2.5f, 0.0f},
    Point2D{0.0f, 2.5f}
    });

    tracker.update({
    Point2D{2.53f, 0.02f},
    Point2D{0.01f, 2.48f}
    });

    ASSERT_EQ(tracker.tracks().size(), 2u);
    EXPECT_EQ(tracker.tracks()[0].id, 0u);
    EXPECT_EQ(tracker.tracks()[1].id, 1u);
    EXPECT_EQ(tracker.tracks()[0].observations, 2u);
    EXPECT_EQ(tracker.tracks()[1].observations, 2u);
}

TEST(ClusterTrackerTest, CreatesNewTrackWhenFarAway)
{
    ClusterTracker tracker(0.5f);

    tracker.update({Point2D{1.0f, 1.0f}});
    tracker.update({Point2D{5.0f, 5.0f}});

    ASSERT_EQ(tracker.tracks().size(), 2u);
    EXPECT_EQ(tracker.tracks()[0].id, 0u);
    EXPECT_EQ(tracker.tracks()[1].id, 1u);
}

TEST(ClusterTrackerTest, RemovesStaleTrack)
{
    ClusterTracker tracker(0.5f, 2);

    tracker.update({Point2D{1.0f, 1.0f}});

    tracker.update({});
    tracker.update({});
    tracker.update({});

    EXPECT_TRUE(tracker.tracks().empty());
}
