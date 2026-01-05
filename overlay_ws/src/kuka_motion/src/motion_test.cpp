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
        MotionTest () : Node("motion_test")
        {
            publisher = this->create_publisher<geometry_msgs::msg::PoseStamped>("/target_pose", 10);
            // Start timer
            timer = this->create_wall_timer(10s, std::bind(&MotionTest::motion_callback, this));
            pose_vector = get_poses();
        }

    private:

    std::vector<geometry_msgs::msg::PoseStamped> get_poses()
    {
        std::vector<geometry_msgs::msg::PoseStamped> pose_vector;

            geometry_msgs::msg::PoseStamped pose;
            pose.header.frame_id = "base_link";

            // Home Pose
            pose.pose.position.x = 1.0;
            pose.pose.position.y = 0.0;
            pose.pose.position.z = 1.819;

            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.035;
            pose.pose.orientation.z = -0.00;
            pose.pose.orientation.w = 0.999;
            pose_vector.push_back(pose);

            // Pose 1: Move Forward by 10cm
            pose.pose.position.x = 1.1;
            pose.pose.position.y = 0.0;
            pose.pose.position.z = 1.819;

            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.035;
            pose.pose.orientation.z = 0.00;
            pose.pose.orientation.w = 0.999;
            pose_vector.push_back(pose);

            // Back to Home Pose
            pose.pose.position.x = 1.0;
            pose.pose.position.y = 0.0;
            pose.pose.position.z = 1.819;

            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.035;
            pose.pose.orientation.z = -0.00;
            pose.pose.orientation.w = 0.999;
            pose_vector.push_back(pose);

            // Move sideways 10cm
            pose.pose.position.x = 1.0;
            pose.pose.position.y = 0.1;
            pose.pose.position.z = 1.819;

            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.035;
            pose.pose.orientation.z = 0.00;
            pose.pose.orientation.w = 0.999;
            pose_vector.push_back(pose);

            // Back to Home Pose
            pose.pose.position.x = 1.0;
            pose.pose.position.y = 0.0;
            pose.pose.position.z = 1.819;

            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.035;
            pose.pose.orientation.z = -0.00;
            pose.pose.orientation.w = 0.999;
            pose_vector.push_back(pose);


            // Move down 20cm
            pose.pose.position.x = 1.0;
            pose.pose.position.y = 0.0;
            pose.pose.position.z = 1.80;

            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.035;
            pose.pose.orientation.z = 0.00;
            pose.pose.orientation.w = 0.999;
            pose_vector.push_back(pose);


            // Back to Home Pose
            pose.pose.position.x = 1.0;
            pose.pose.position.y = 0.0;
            pose.pose.position.z = 1.819;

            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.035;
            pose.pose.orientation.z = -0.00;
            pose.pose.orientation.w = 0.999;
            pose_vector.push_back(pose);

           // Rotate around x (normalized)
            pose.pose.orientation.x = 0.094;
            pose.pose.orientation.y = -0.329;
            pose.pose.orientation.z = 0.0;
            pose.pose.orientation.w = 0.939;
            pose_vector.push_back(pose);

            // Back to Home (normalized)
            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.035;
            pose.pose.orientation.z = -0.00;
            pose.pose.orientation.w = 0.999;
            pose_vector.push_back(pose);

            // Rotate around y (normalized)
            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.243;
            pose.pose.orientation.z = 0.0;
            pose.pose.orientation.w = 0.970;
            pose_vector.push_back(pose);

            // Back to Home (normalized)
            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.035;
            pose.pose.orientation.z = -0.00;
            pose.pose.orientation.w = 0.999;
            pose_vector.push_back(pose);

            // Rotate around z (normalized)
            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.329;
            pose.pose.orientation.z = 0.094;
            pose.pose.orientation.w = 0.939;
            pose_vector.push_back(pose);

            // Back to Home (normalized)
            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = -0.035;
            pose.pose.orientation.z = -0.00;
            pose.pose.orientation.w = 0.999;
            pose_vector.push_back(pose);


            return pose_vector;
    }

    void motion_callback()
        {
            if(i >= pose_vector.size())
            {
                RCLCPP_INFO(get_logger(), "All poses published. Stopping timer ...");
                timer->cancel();
                return;
            }

            auto pose = pose_vector[i];
            pose.header.stamp = now();
            publisher->publish(pose);


            RCLCPP_INFO(
            get_logger(),
            "Published Pose %zu: [%.3f %.3f %.3f] | [%.3f %.3f %.3f %.3f] ",
            i,
            pose.pose.position.x,
            pose.pose.position.y,
            pose.pose.position.z,
            pose.pose.orientation.x,
            pose.pose.orientation.y,
            pose.pose.orientation.z,
            pose.pose.orientation.w);
                
            i++;
        }

        
        rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr publisher;
        rclcpp::TimerBase::SharedPtr timer;
        std::vector<geometry_msgs::msg::PoseStamped> pose_vector;
        size_t i {0};
};


int main (int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotionTest>());
    rclcpp::shutdown();
    return 0;
}



