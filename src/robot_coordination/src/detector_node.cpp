#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>
#include <memory>
#include <chrono>
#include <string>
#include "coordination_interfaces/msg/raw_detection.hpp"

using namespace std::chrono_literals;

class DetectorNode : public rclcpp::Node
{
public:
    DetectorNode() : Node("detector_node")
    {
        // 声明参数
        this->declare_parameter<double>("x", 0.0);
        this->declare_parameter<double>("y", 0.0);
        this->declare_parameter<double>("z", 0.0);
        this->declare_parameter<std::string>("robot_name", "robot_a");
        
        // 创建发布者
        publisher_ = this->create_publisher<coordination_interfaces::msg::RawDetection>(
            "/robot_a/raw_detection", 10);
        
        // 创建定时器，10Hz
        timer_ = this->create_wall_timer(
            100ms, std::bind(&DetectorNode::timer_callback, this));
        
        // 添加参数回调，支持动态参数修改
        parameters_callback_handle_ = this->add_on_set_parameters_callback(
            std::bind(&DetectorNode::parameters_callback, this, std::placeholders::_1));
    }

private:
    void timer_callback()
    {
        auto msg = coordination_interfaces::msg::RawDetection();
        
        // 从参数获取坐标值
        msg.point.x = this->get_parameter("x").as_double();
        msg.point.y = this->get_parameter("y").as_double();
        msg.point.z = this->get_parameter("z").as_double();
        msg.robot_name = this->get_parameter("robot_name").as_string();
        
        publisher_->publish(msg);
    }
    
    rcl_interfaces::msg::SetParametersResult parameters_callback(
        const std::vector<rclcpp::Parameter> &parameters)
    {
        
        for (auto param : parameters)
        {
            if (param.get_name() == "x" || param.get_name() == "y" || param.get_name() == "z")
            {
                RCLCPP_INFO(this->get_logger(), "参数 %s 已修改为: %f", 
                           param.get_name().c_str(), param.as_double());
            }
        }

        rcl_interfaces::msg::SetParametersResult result;
        result.successful = true;
        return result;
    }
    
    rclcpp::Publisher<coordination_interfaces::msg::RawDetection>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr parameters_callback_handle_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DetectorNode>());
    rclcpp::shutdown();
    return 0;
}