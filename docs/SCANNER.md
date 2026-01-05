# Scanner

## Getting Started

´´´sh
  ros2 run sick_safetyscanners2 sick_safetyscanners2_node --ros-args \
    -r __node:=sick_back \
    -p sensor_ip:=192.168.1.23 \
    -p host_ip:=192.168.1.27 \
    -p frame_id:=nanoscan3_back_left \
    -r /scan:=/scan_back
´´´

´´´sh
  ros2 run sick_safetyscanners2 sick_safetyscanners2_node --ros-args \
    -r __node:=sick_front \
    -p sensor_ip:=192.168.1.22 \
    -p host_ip:=192.168.1.27 \
    -p frame_id:=nanoscan3_front_right \
    -r /scan:=/scan_front
´´´