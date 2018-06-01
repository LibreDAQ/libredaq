/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  ROS driver for LibreDAQ
 *  Copyright (c) 2018, Jose Luis Blanco Claraco
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#include <cstdlib>
#include <libredaq.h>
#include <mutex>
#include <numeric> // std::accumulate()
#include <ros/ros.h>
#include <sstream>
#include <std_msgs/String.h>
#include <stdio.h>
#include <vector>

#include "libredaq/EncoderDecimatedSpeed.h"
#include <sensor_msgs/JointState.h>

// (Default=20 Hz) Rate for publishing encoder states
double PUBLISH_RATE = 20; // [Hz]
// (Default=10) Number of samples for the sliding window average filter of
// speeds.
int SPEED_FILTER_SAMPLES_LEN = 10;
// (Default=1) Number of "ITERATE" loops without any new encoder tick before
// resetting the filtered average velocities.
int SPEED_FILTER_IDLE_ITER_LOOPS_BEFORE_RESET = 1;

struct TStatePerChannel {
  int tickPos = .0;
  double instantaneousSpeed = .0;
  std::vector<double> speeds_buffer;
  bool speed_buffer_updated = false;
  unsigned int loops_without_update_speed_buffer = 0;
};
std::vector<TStatePerChannel> encoder_states;
std::mutex encoder_states_mux;

// Callback for ENCODERS data
void my_callback_ENC(const libredaq::TCallbackData_ENC &data) {
  std::lock_guard<std::mutex> lock(encoder_states_mux);

  static double last_tim = .0;
  const double At = data.device_timestamp - last_tim;
  last_tim = data.device_timestamp;
  const double At_inv = At > .0 ? 1.0 / At : .0;

  const size_t NUM_ENCODERS =
      sizeof(data.enc_ticks) / sizeof(data.enc_ticks[0]);
  encoder_states.resize(NUM_ENCODERS);
  for (size_t i = 0; i < NUM_ENCODERS; i++) {
    TStatePerChannel &spc = encoder_states[i];
    const auto positionChange = data.enc_ticks[i] - spc.tickPos;
    spc.tickPos = data.enc_ticks[i];

    spc.instantaneousSpeed = positionChange * At_inv;
    spc.speeds_buffer.push_back(spc.instantaneousSpeed);
    spc.speed_buffer_updated = true;
    spc.loops_without_update_speed_buffer = 0;
  }

  ROS_INFO_THROTTLE(0.5, "TIME: %15.7f ENCODERS: %8f %8f %8f %8f\n",
                    data.device_timestamp, (double)data.enc_ticks[0],
                    (double)data.enc_ticks[1], (double)data.enc_ticks[2],
                    (double)data.enc_ticks[3]);
}

int main(int argc, char *argv[]) {
  try {

    ros::init(argc, argv, "libredaq");
    ros::NodeHandle n;
    ros::NodeHandle nh("~");

    std::vector<std::string> joint_names = {"joint0", "joint1", "joint2",
                                            "joint3"};
    std::vector<double> joint_tick2rad = {4, 1.0};
    for (unsigned int i = 0; i < joint_names.size(); i++) {
      char str[100];
      sprintf(str, "joint%u_name", i);
      nh.getParam(str, joint_names[i]);

      sprintf(str, "joint%u_tick2rad", i);
      nh.getParam(str, joint_tick2rad[i]);

      ROS_INFO("Channel %u: '%s'='%s'", i, str, joint_names[i].c_str());
    }

    std::string serial_port = "ttyACM0";
    nh.getParam("serial_port", serial_port);

    std::string frame_id;
    nh.getParam("frame_id", frame_id);
    ROS_INFO("frame_id = '%s'", frame_id.c_str());
    nh.getParam("PUBLISH_RATE", PUBLISH_RATE);
    nh.getParam("SPEED_FILTER_SAMPLES_LEN", SPEED_FILTER_SAMPLES_LEN);
    nh.getParam("SPEED_FILTER_IDLE_ITER_LOOPS_BEFORE_RESET",
                SPEED_FILTER_IDLE_ITER_LOOPS_BEFORE_RESET);

    // First time: create publishers:
    const std::string topic_path = "joint_states";
    ROS_INFO("Publishing state to topic: %s", topic_path.c_str());

    ros::Publisher encoder_pub =
        n.advertise<sensor_msgs::JointState>(topic_path, 100);
    std::vector<ros::Publisher> encoder_decimspeed_pubs;

    // Declare the object that represents one LibreDAQ physical device
    libredaq::Device daq;

    // Establish communications:
    daq.connect_serial_port(serial_port);
    //    ROS_ERROR("Problem waiting for attachment: %s", sError.c_str());
    //    return 1; // !=0 exit status means error in main()

    daq.set_callback_ENC(&my_callback_ENC);

    printf("Starting ENCODERS task...\n");
    daq.start_task_encoders(PUBLISH_RATE * 4);

    ros::Rate rate(PUBLISH_RATE);
    ROS_INFO("Publishing encoder states at %.03f Hz", PUBLISH_RATE);
    while (ros::ok()) {
      ros::spinOnce();
      rate.sleep();

      // Publish:
      {
        std::lock_guard<std::mutex> lock(encoder_states_mux);
        const unsigned int N = encoder_states.size();

        // First time? We need to create decimated speed publisher here,
        // once we know how many channel we have. Also, they must be independent
        // for each channel due to the unsynchronous nature of the filtering
        // algorithm:
        if (encoder_decimspeed_pubs.size() != N) {
          encoder_decimspeed_pubs.resize(N);
          for (unsigned int i = 0; i < N; i++) {
            std::string s = topic_path;
            char buf[100];
            sprintf(buf, "_ch%u_decim_speed", i);
            s += buf;
            ROS_INFO("Publishing decimated speed of channel %u to topic: %s", i,
                     s.c_str());
            encoder_decimspeed_pubs[i] =
                n.advertise<libredaq::EncoderDecimatedSpeed>(s, 10);
          }
        }

        sensor_msgs::JointState js_msg;
        static uint32_t seq_cnt = 0;
        js_msg.header.seq = (seq_cnt++);
        js_msg.header.stamp = ros::Time::now();
        js_msg.header.frame_id = frame_id;

        js_msg.name.resize(N);
        for (unsigned int i = 0; i < std::min<size_t>(joint_names.size(), N);
             i++)
          js_msg.name[i] = joint_names[i];

        js_msg.position.resize(N);
        js_msg.velocity.resize(N);
        js_msg.effort.clear();

        for (unsigned int i = 0; i < N; i++) {
          TStatePerChannel &spc = encoder_states[i];

          js_msg.position[i] = spc.tickPos * joint_tick2rad[i];
          js_msg.velocity[i] = spc.instantaneousSpeed * joint_tick2rad[i];

          spc.instantaneousSpeed = 0; // Reset speed

          if (SPEED_FILTER_SAMPLES_LEN > 0) {
            if (!spc.speed_buffer_updated) {
              if (int(++spc.loops_without_update_speed_buffer) >=
                  SPEED_FILTER_IDLE_ITER_LOOPS_BEFORE_RESET) {
                libredaq::EncoderDecimatedSpeed e;
                e.header.stamp = ros::Time::now();
                e.header.frame_id = frame_id;
                e.avr_speed = .0;
                encoder_decimspeed_pubs[i].publish(e);
              }
            } else {
              spc.loops_without_update_speed_buffer = 0;

              if (int(spc.speeds_buffer.size()) >= SPEED_FILTER_SAMPLES_LEN) {
                const double avrg =
                    std::accumulate(spc.speeds_buffer.begin(),
                                    spc.speeds_buffer.end(), 0.0) /
                    spc.speeds_buffer.size();
                spc.speeds_buffer.clear();

                libredaq::EncoderDecimatedSpeed e;
                e.header.stamp = ros::Time::now();
                e.header.frame_id = frame_id;
                e.avr_speed = avrg * joint_tick2rad[i];
                encoder_decimspeed_pubs[i].publish(e);
              }
            }
          }
        }

        encoder_pub.publish(js_msg);

      } // end lock guard
    }   // end while ros::ok()

    return 0;

  } catch (std::exception &e) {
    ROS_ERROR("Exception: %s", e.what());
    return 1;
  }
}
