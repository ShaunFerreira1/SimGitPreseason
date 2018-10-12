#include "sim_main.h"
#include "motor_window.h"
#include "tank_window.h"
#include "xbox_window.h"

#include "usage.h"

#include "hal/HAL.h"
#include "mockdata/PWMData.h"
#include "opencv2/highgui/highgui.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <thread>

static std::vector<sim_motor_window> _motors;
static std::vector<tank_window>      _tanks;
static sim_xbox_window               _xbox;

void all_windows(std::function<void(window &)> cb) {
  cb(_xbox);
  for (auto it = _motors.begin(); it != _motors.end(); ++it) {
    cb(*it);
  }
  for (auto it = _tanks.begin(); it != _tanks.end(); ++it) {
    cb(*it);
  }
}

void sim_main::run() {
  std::cout << "[SIM] Simulation Starting..." << std::endl;
  HAL_Initialize(500, 0);

  _motors.reserve(10);
  for (int i = 0; i < 10; i++) _motors.emplace_back(i);

  _xbox.start();

  usage::on_drivetrain([](int id) {
    std::pair<int, int> motors = usage::get_drivetrain(id);
    std::cout << "[SIM] Found Drivetrain: " << id << " Left: " << motors.first << " Right: " << motors.second
              << std::endl;
    _tanks.emplace_back(motors.first, motors.second);
    _tanks.back().start();
  });

  std::cout << "[SIM] Starting UI Thread..." << std::endl;
  std::thread thread([]() {
    while (true) {
      all_windows([](window &win) { win.update(); });

      cv::waitKey(static_cast<int>(1000.0 / 45.0));
    }
  });
  thread.detach();
  std::cout << "[SIM] Simulation Initialized!" << std::endl;
}