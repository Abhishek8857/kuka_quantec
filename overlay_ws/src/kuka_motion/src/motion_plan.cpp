    #include <rclcpp/rclcpp.hpp>
    #include <moveit/move_group_interface/move_group_interface.h>
    #include <geometry_msgs/msg/pose_stamped.hpp>
    #include <vector>
    #include <mutex>

    std::vector<geometry_msgs::msg::Pose> pose_queue;
    std::mutex pose_mutex;

    void poseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr msg)
    {
        std::lock_guard<std::mutex> lock(pose_mutex);
        pose_queue.push_back(msg->pose);

        RCLCPP_INFO(
            rclcpp::get_logger("pose_listener"),
            "Received pose (queue size = %zu)",
            pose_queue.size()
        );
    }

    int main(int argc, char** argv)
    {
        rclcpp::init(argc, argv);
        auto node = rclcpp::Node::make_shared("motion_plan_pose_sequence");

        // Subscriber
        auto sub = node->create_subscription<geometry_msgs::msg::PoseStamped>(
            "/target_pose",
            10,
            poseCallback
        );

        // Executor
        rclcpp::executors::SingleThreadedExecutor executor;
        executor.add_node(node);

        // Give ROS + MoveIt time
        rclcpp::sleep_for(std::chrono::seconds(2));

        static const std::string PLANNING_GROUP = "manipulator";
        moveit::planning_interface::MoveGroupInterface move_group(node, PLANNING_GROUP);

        move_group.setPlanningPipelineId("pilz_industrial_motion_planner");
        move_group.setPlannerId("PTP");
        move_group.setMaxVelocityScalingFactor(0.05);
        move_group.setMaxAccelerationScalingFactor(0.05);
        // move_group.setPoseReferenceFrame("base_link");
        // move_group.setPlanningTime(10.0);
        // move_group.setGoalOrientationTolerance(0.01);
        // move_group.setNumPlanningAttempts(1);
        // move_group.setStartStateToCurrentState();

        RCLCPP_INFO(node->get_logger(), "Waiting for poses on /target_pose...");

        while (rclcpp::ok())
        {
            // Handle incoming pose messages
            executor.spin_some();   

            geometry_msgs::msg::Pose target_pose;
            bool has_pose = false;

            {
                std::lock_guard<std::mutex> lock(pose_mutex);
                if (!pose_queue.empty())
                {
                    target_pose = pose_queue.front();
                    pose_queue.erase(pose_queue.begin());
                    has_pose = true;
                }
            }

            if (!has_pose)
            {
                rclcpp::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            RCLCPP_INFO(
                node->get_logger(),
                "Planning to pose: x=%.3f y=%.3f z=%.3f",
                target_pose.position.x,
                target_pose.position.y,
                target_pose.position.z
            );

            move_group.setPoseTarget(target_pose);

            moveit::planning_interface::MoveGroupInterface::Plan plan;
            auto plan_result = move_group.plan(plan);

            if (plan_result != moveit::core::MoveItErrorCode::SUCCESS)
            {
                RCLCPP_ERROR(node->get_logger(), "Planning failed.");
                move_group.clearPoseTargets();
                continue;
            }

            RCLCPP_INFO(node->get_logger(), "Plan successful!");

            // User confirmation
            std::cout << "\n>>> PLAN READY\n";
            std::cout << "Execute this motion? (ENTER = yes, 's' + ENTER = skip)\n";

            std::string input;
            std::getline(std::cin, input);

            if (input == "s" || input == "S")
            {
                RCLCPP_WARN(node->get_logger(), "Execution skipped.");
                move_group.clearPoseTargets();
                continue;
            }

            RCLCPP_INFO(node->get_logger(), "Executing...");

            auto exec_result = move_group.execute(plan);
            if (exec_result != moveit::core::MoveItErrorCode::SUCCESS)
            {
                RCLCPP_ERROR(node->get_logger(), "Execution failed.");
                move_group.clearPoseTargets();
                continue;
            }

            RCLCPP_INFO(node->get_logger(), "Execution finished.");
            move_group.clearPoseTargets();
        }

        rclcpp::shutdown();
        return 0;
    }
