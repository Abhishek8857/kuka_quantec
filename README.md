# Kuka Quantec ROS2 

## Getting Started

For more details, see the [KUKA RSI Wiki](https://github.com/kroshu/kuka_drivers/wiki/2_KSS_RSI).

## IP Setup

Controlling the KUKA Robot with ROS heavily depends on the correct IP Configuration being set. If the IPs are incorrectly configured, the driver does not start and the errors are cryptic in the least where the IP configuration is failing.

Currently, the stack has been developed with the IP config : `192.168.2.27` where the subnet is `2` and the host address is `27`. 

Configure the Network config by adding the KUKA robot as a new wired connection. 
- Go to wired settings on Ubuntu
- Select add new wired connection
- Click on IPv4 tab and select manual option
- In the Address add `192.168.2.27` and subnet as `255.255.255.0` and then click on add

**Having the same IP Address set on the Robot is critical for the Robot Sensor Interface (RSI) to communicate with the client machine. These have to be changed  in two locations and can be set manually from WorkVisual or directly from the Teachpendant**

1. Go to file location: `KRC:\R1\Program\Test_Program` and open the file `rsi_ethernet.xml`
    1.  Change the IP address in `<IP_NUMBER>` tag to `192.168.2.27`.
    2.  Change the PORT to `<PORT>` to 59152.
    3.  Do the same for the file `rsi_gpio_ethernet.xml`.

2. Now go to file location: `C:\KRC\ROBOTER\Config\User\Common\SensorInterface\` and repeat the process for `rsi_ethernet.xml`
    1.  Change the IP address in `<IP_NUMBER>` tag to `192.168.2.27`.
    2.  Change the PORT to `<PORT>` to 59152.
    3.  Do the same for the file `rsi_gpio_ethernet.xml`

<!-- The Files in both locations should look like in the image below

![rsi_ethernet](/status/rsi_ethernet_.xml.jpeg)
![rsi_gpio](/status/rsi_gpio_ethernet.xml.jpeg) -->

**Please check the status folder in this Repository for the complete network configuration of the Robot if you want to see the complete network setup**

### 1. Clone the Repo

Clone the Repo normally 
``` sh 
git clone --recurse-submodules  -b devel_abhishek https://git-ce.rwth-aachen.de/wzl-mq-ms/docker-ros/ros2/kuka-quantec-arm-ros2.git
```

or clone with ssh

``` sh 
git clone --recurse-submodules -b devel_abhishek git@git-ce.rwth-aachen.de:wzl-mq-ms/docker-ros/ros2/kuka-quantec-arm-ros2.git
```
- If you’ve already cloned the repository without submodules, you can initialize and update the submodules like this:

```sh
git submodule update --init --recursive
```

### 2. Build docker image

Build the Docker image using the provided Dockerfile. This command must be run from the root of the repository where the Dockerfile is located. The name of the images is set in `image_name.cfg`. You can change it to your preferred name if you want, the default name is set to `kuka-quantec-ros:latest`

```sh
bash docker_build.sh
```
- This will create a Docker image with the default name from the file, which builds the packages in the `overlay_ws` which has all the KUKA packages along with Moveit2 and `colcon_ws` which contains a downgraded Rviz2 submodule to fix compaatibility issues.


### 3. Run the Docker Container

Once the Docker image is built, you can run the container interactively using:

```sh
cd docker_run/
bash docker_run.sh
```
You will see the following errors show up. 

`not found: "/overlay_ws/install/kuka_external_control_sdk/share/kuka_external_control_sdk/local_setup.bash"`
`not found: "/overlay_ws/install/kuka_external_control_sdk_examples/share/kuka_external_control_sdk_examples/local_setup.bash"`

These errors have not yet been solved but do not cause any issue in running the robot

### 4. Launch the KUKA Driver 

```sh
ros2 launch kuka_bringup driver.launch.py
```
- This will start the 3 controllers `joint_state_broadcaster`, `joint_trajectory_controller` & `gpio_controller`. If you want to launch without the `gpio_controller`, use the `use_gpio:=false` as shown below while launching the driver.

```sh
ros2 launch kuka_bringup driver.launch.py use_gpio:=false
```

### 5. Configure the Robot Manager
- Open another terminal and open another instance of the container by running the following command and source the workspace

```sh
ros2 lifecycle set robot_manager configure
```
- You should get `Transition sucessful` message on sucessful configuration. Now we need to activate the manager.

```sh
ros2 lifecycle set robot_manager activate
```
- After the activation, a timeout of 30 seconds is provided to execute the program file on the teachpendant.

### 6. Execute the Program on Teachpendant

1.  On the Teachpendant navigate to `KRC:\R1\Program\Test_Program\`

2.  If you launched the driver with `use_gpio:=false`, you need to select and execute the `rsi_gpio_joint_pose_12ms.src` on the Teachpendant.

![rsi_joint_pos_12ms.src](/status/rsi_joint_pos_12ms.jpeg)

3. If you launched the driver without any arguments, you need to select and execute `rsio_gpio_example.src` on the Teachpendant

![rsi_gpio_example](/status/rsi_gpio_example.jpeg)

You will have to execute the program multiple times as it steps through the code and reaches `RSI_MOVECORR()` line. The program should then idle on that line with with all status symbols showing green as shown in the images

![Program Status](/status/program_status.jpeg)

If everything runs accordingly, you should see `Transisiton successful` message again indicating successful configuration.

### 7. Launch Moveit2

If all the other steps ran smoothly, you should now be ready to launch Moveit2 by running the following command

```sh
ros2 launch kuka_bringup kuka.launch.py
```

This will launch Rviz2 with Moveit2 and Motion Planning already loaded. You can now control the robot through it.

### Changes done in packages:
- `kuka_drivers` : 
    1. GPIO code has been uncommented to allow input/output commands for the TCP
    2. Timeout has been changed from 10 to 30 in `robot_manager_node_rsi_only.cpp` and `hardware_interface_rsi_only.cpp`
- `kuka_external_control_sdk`
    1. Added IP address of the PC in `rsi_ethernet.xml` 

## TODO's
- [x] Edit the URDF with the updated `kuka_robot_description` files
- [x] Create Moveit config files for the arm
- [x] Integrate the Omnimove platform with the arm
- [x] Integrate the LiDAR sensors with the platform
- [x] Modify the Platform URDF to make it possible to translate in X, Y and rotate in Z Axes
- [x] Test Lidar Sensors and confirm if they are recieving data


