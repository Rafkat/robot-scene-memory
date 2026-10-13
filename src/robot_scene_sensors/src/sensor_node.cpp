
#include <memory>

#include "rclcpp/rclcpp.hpp"

class SensorNode : public rclcpp::Node {
public:
    SensorNode()
        : Node("sensor_node") {
        RCLCPP_INFO(
            this->get_logger(),
            "Robot Scene Sensors node started!"
        );
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SensorNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}

