//
// Created by Рафкат Галеев on 20/10/2026.
//
#include "robot_scene_sensors/cluster_geometry.hpp"

#include <algorithm>
#include <stdexcept>

namespace robot_scene_sensors {

    ClusterGeometry compute_cluster_geometry(
        const Cluster & cluster
    ) {
        if (cluster.empty()) {
            throw std::invalid_argument("Cluster must not be empty");
        }

        ClusterGeometry result{};

        result.point_count = cluster.size();

        result.bbox.min_x = cluster[0].x;
        result.bbox.max_x = cluster[0].x;
        result.bbox.min_y = cluster[0].y;
        result.bbox.max_y = cluster[0].y;

        double sum_x = 0.0;
        double sum_y = 0.0;

        for (const auto & point : cluster) {
            sum_x += point.x;
            sum_y += point.y;

            result.bbox.min_x = std::min(result.bbox.min_x, point.x);
            result.bbox.max_x = std::max(result.bbox.max_x, point.x);
            result.bbox.min_y = std::min(result.bbox.min_y, point.y);
            result.bbox.max_y = std::max(result.bbox.max_y, point.y);
        }

        result.centroid.x = static_cast<float>(sum_x / static_cast<double>(cluster.size()));
        result.centroid.y = static_cast<float>(sum_y / static_cast<double>(cluster.size()));

        return result;
    }

}