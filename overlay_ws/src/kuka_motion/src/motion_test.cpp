#include <rclcpp/rclcpp.hpp>
#include <memory>
#include <chrono>
#include <functional>
#include <string>
#include <geometry_msgs/msg/pose_stamped.hpp>

using namespace std::chrono_literals;


class MotionTest : public rclcpp::Node
{
    public:
        MotionTest () : Node("Motion_test")
        {
            publisher = this->create_publisher<geometry_msgs::msg::PoseStamped>("/target_pose", 10);
            timer = this->create_wall_timer(500ms, std::bind(&MotionTest::motion_callback, this));
        }

    private:
        void motion_callback()
        {
            geometry_msgs::msg::PoseStamped pose;

            pose.header.stamp = this->now();
            pose.header.frame_id = "base_link";

            pose.pose.position.x = 1.944;
            pose.pose.position.y = 0.043;
            pose.pose.position.z = 1.733;

            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.705;
            pose.pose.orientation.z = 0.014;
            pose.pose.orientation.w = 0.707;

            publisher->publish(pose);

            RCLCPP_INFO(this->get_logger(), "Published Pose: [%.3f %.3f %.3f]",
                                            pose.pose.position.x, pose.pose.position.y, pose.pose.position.z);
            timer->cancel();
        }

        rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr publisher;
        rclcpp::TimerBase::SharedPtr timer;
};


int main (int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotionTest>());
    rclcpp::shutdown();
    return 0;
}



