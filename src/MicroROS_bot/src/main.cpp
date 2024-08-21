#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include "motor.h"
#include "imu.h"

rcl_subscription_t cmd_vel_sub;
geometry_msgs__msg__Twist cmd_vel_msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

void cmd_vel_callback(const void *msg_recv) {
    const geometry_msgs__msg__Twist *recieved_data = (const geometry_msgs__msg__Twist *) msg_recv;
    float linear_vel = recieved_data->linear.x;
    float angular_vel = recieved_data->angular.z;

    Serial.print(linear_vel); Serial.print(" / "); Serial.println(angular_vel);

    controlMotors(linear_vel, angular_vel);
}

void setup() {
    Serial.begin(115200);
    IPAddress agent_ip(192, 168, 31, 204);
    size_t agent_port = 8888;
    char ssid[] = "ROS";
    char psk[] = "1234567890";
    set_microros_wifi_transports(ssid, psk, agent_ip, agent_port);

    delay(2000);

    allocator = rcl_get_default_allocator();
    rclc_support_init(&support, 0, NULL, &allocator);
    rclc_node_init_default(&node, "cmd_vel_sub", "", &support);

    rclc_subscription_init_default(
        &cmd_vel_sub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "/cmd_vel"
    );

    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_subscription(&executor, &cmd_vel_sub, &cmd_vel_msg, &cmd_vel_callback, ON_NEW_DATA);

    Motor_Init();
}

void loop() {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    delay(100);
}
