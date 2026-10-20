#pragma once

#include <cstddef>

#include "robot_scene_sensors/lidar_clustering.hpp"

namespace robot_scene_sensors {

    struct BoundingBox2D {
        float min_x;
        float min_y;
        float max_x;
        float max_y;
    };

    struct ClusterGeometry {
        Point2D centroid;
        BoundingBox2D bbox;
        std::size_t point_count;
    };

    ClusterGeometry compute_cluster_geometry(
        const Cluster & cluster
    );

}