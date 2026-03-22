#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <memory>
#include <string>
#include "coordination_interfaces/msg/raw_detection.hpp"
#include "coordination_interfaces/msg/coord_task.hpp"

class CenterNode : public rclcpp::Node
{
public:
    CenterNode() : Node("center_node")
    {
        // 创建TF2缓冲区和管理器
        tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
        
        // 创建静态广播发布器并发布静态变换
        static_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
        publish_static_transform();
        
        // 创建订阅者
        subscription_ = this->create_subscription<coordination_interfaces::msg::RawDetection>(
            "/robot_a/raw_detection", 10,
            std::bind(&CenterNode::detection_callback, this, std::placeholders::_1));
        
        // 创建发布者到Robot B
        publisher_ = this->create_publisher<coordination_interfaces::msg::CoordTask>(
            "/robot_b/mission_goal", 10);
        
        RCLCPP_INFO(this->get_logger(), "Center Node已启动");
    }

private:
    void publish_static_transform()
    {
        geometry_msgs::msg::TransformStamped static_transform;
        
        static_transform.header.stamp = this->get_clock()->now();
        static_transform.header.frame_id = "map";
        static_transform.child_frame_id = "camera_link";
        
        // 设置变换: x=1.0, z=0.5, 无旋转
        static_transform.transform.translation.x = 1.0;
        static_transform.transform.translation.y = 0.0;
        static_transform.transform.translation.z = 0.5;
        
        static_transform.transform.rotation.x = 0.0;
        static_transform.transform.rotation.y = 0.0;
        static_transform.transform.rotation.z = 0.0;
        static_transform.transform.rotation.w = 1.0;   //旋转角度为0度
        
        static_broadcaster_->sendTransform(static_transform);
        RCLCPP_INFO(this->get_logger(), "已发布静态变换: map -> camera_link");
    }
    
    void detection_callback(const coordination_interfaces::msg::RawDetection::SharedPtr raw_msg)
    {
        RCLCPP_DEBUG(this->get_logger(), "收到原始检测: %s 坐标: (%.2f, %.2f, %.2f)",
                    raw_msg->robot_name.c_str(), 
                    raw_msg->point.x, raw_msg->point.y, raw_msg->point.z);
        
        // 创建带坐标系的信息
        geometry_msgs::msg::PointStamped point_camera;
        point_camera.header.stamp = this->get_clock()->now();
        point_camera.header.frame_id = "camera_link";
        point_camera.point = raw_msg->point;
        
        // 等待变换可用
        if (!tf_buffer_->canTransform("map", "camera_link", tf2::TimePointZero, tf2::durationFromSec(1.0)))
        {
            RCLCPP_WARN(this->get_logger(), "无法获取map到camera_link的变换");
            return;
        }
        
        // 执行坐标变换,参数含义：待变换的原始点，目标坐标系，等待时间
        geometry_msgs::msg::PointStamped point_map = tf_buffer_->transform(point_camera, "map", tf2::durationFromSec(1.0));
        
        // 根据x坐标决定动作
        std::string action = determine_action(point_map.point.x);
        
        // 创建任务消息
        coordination_interfaces::msg::CoordTask task_msg;
        task_msg.map_point = point_map.point;
        task_msg.action = action;
        
        // 发布到Robot B
        publisher_->publish(task_msg);
        
        RCLCPP_INFO(this->get_logger(),
            "坐标变换: camera(%.2f, %.2f, %.2f) -> map(%.2f, %.2f, %.2f) , 动作: %s",
            raw_msg->point.x, raw_msg->point.y, raw_msg->point.z,
            point_map.point.x, point_map.point.y, point_map.point.z,
            action.c_str());
    }
    
    std::string determine_action(double x)
    {
        if (x > 2.0)
            return "Wait";
        else  //x <= 2.0
            return "Catch";
    }
    
    std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> static_broadcaster_;
    rclcpp::Subscription<coordination_interfaces::msg::RawDetection>::SharedPtr subscription_;
    rclcpp::Publisher<coordination_interfaces::msg::CoordTask>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CenterNode>());
    rclcpp::shutdown();
    return 0;
}