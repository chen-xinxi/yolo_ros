#include <rclcpp/rclcpp.hpp>
#include "coordination_interfaces/msg/coord_task.hpp"
#include "coordination_interfaces/srv/get_status.hpp"
#include <memory>
#include <string>

class ExecutorNode : public rclcpp::Node
{
public:
    ExecutorNode() : Node("executor_node"), current_status_("Ready")
    {
        // 创建订阅者
        subscription_ = this->create_subscription<coordination_interfaces::msg::CoordTask>(
            "/robot_b/mission_goal", 10,
            std::bind(&ExecutorNode::mission_callback, this, std::placeholders::_1));
        
        // 创建服务
        service_ = this->create_service<coordination_interfaces::srv::GetStatus>(
            "get_status",
            std::bind(&ExecutorNode::status_callback, this, 
                     std::placeholders::_1, std::placeholders::_2));
    }

private:
    void mission_callback(const coordination_interfaces::msg::CoordTask::SharedPtr msg)
    {
        
        
        // 打印接收到的指令
        RCLCPP_INFO(this->get_logger(), "Robot B收到指令: [%s]", msg->action.c_str());
        RCLCPP_INFO(this->get_logger(), "目标坐标: [%.3f, %.3f, %.3f]", 
                   msg->map_point.x, msg->map_point.y, msg->map_point.z);
        
        // 模拟执行任务
        if (msg->action == "Catch")
        {
            current_status_ = "Working";
        }
        else if (msg->action == "Wait")
        {
            current_status_ = "Ready";
        }
    }
    
    void status_callback(
        const std::shared_ptr<coordination_interfaces::srv::GetStatus::Request> request,
        std::shared_ptr<coordination_interfaces::srv::GetStatus::Response> response)
    {
        (void)request;  // 避免未使用参数警告
        response->status = current_status_;
        RCLCPP_INFO(this->get_logger(), "状态查询请求响应: %s", response->status.c_str());
    }
    
    std::string current_status_;
    rclcpp::Subscription<coordination_interfaces::msg::CoordTask>::SharedPtr subscription_;
    rclcpp::Service<coordination_interfaces::srv::GetStatus>::SharedPtr service_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ExecutorNode>());
    rclcpp::shutdown();
    return 0;
}