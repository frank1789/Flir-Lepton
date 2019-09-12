#include <unistd.h>
#include <QApplication>
#include <cstdint>
#include <iostream>
#include <memory>
#include "leptoncamera.h"
#include "logger.h"
#include "mainwindow.hpp"
#include "thermalimage.h"

std::atomic_bool stop{false};

void loop_sensor() {
  LeptonCamera cam;
  ThermalImage image(cam);
  std::cout << "Detected camera: " << cam.LeptonVersion() << "\n";
  usleep(5e+6);
#if LOGGER
  LOG(INFO, "start lepton sensor")
  LOG(INFO, "allocate frame buffer")
  auto dimension = [](const uint32_t& a, const uint32_t& b) { return a * b; };
  LOG(DEBUG, "buffer size: %d", dimension(cam.width(), cam.height()))
#endif
  QVector<uint8_t> qimgU8(cam.width() * cam.height());
  QVector<uint16_t> qimgU16(cam.width() * cam.height());
  std::vector<uint8_t> imgU8(cam.width() * cam.height());
  std::vector<uint16_t> imgU16(cam.width() * cam.height());

  // start cycle thread thermal cmaera
  cam.start();

  while (!stop) {
    // long cycle
    auto temperature = cam.SensorTemperature();
#if LOGGER
    LOG(DEBUG, "temperature acquired: %lf", temperature)
#endif
    cam.getFrameU8(imgU8);
    cam.getFrameU16(imgU16);
    // if (cam.hasFrame())
    // {
    image.setImage(imgU8);
    //      image.save_raw_file();
    image.save_pgm_file();
#if LOGGER
    LOG(INFO, "camera has frame")
    LOG(INFO, "save on file")
#endif
  }

  //         if (req_msg.req_cmd == CMD_FRAME_U8) {
  //             lePi.getFrameU8(imgU8);
  //             memcpy(resp_msg.frame, imgU8.data(), imgU8.size());
  //             resp_msg.bpp = 1;
  //         } else {
  //             lePi.getFrameU16(imgU16);
  //             memcpy(resp_msg.frame, imgU16.data(), imgU16.size() * 2);
  //             resp_msg.bpp = 2;
  //         }
  //         resp_msg.req_status = STATUS_FRAME_READY;
  //         resp_msg.height = lePi.height();
  //         resp_msg.width = lePi.width();
  //         break;
  //     }
  //     case REQUEST_I2C: {
  //         resp_msg.req_type = REQUEST_I2C;
  //         if (lePi.sendCommand(static_cast<LeptonI2CCmd>(req_msg.req_cmd),
  //                              resp_msg.frame)) {
  //             resp_msg.req_status = STATUS_I2C_SUCCEED;
  //         }
  //         else {
  //             resp_msg.req_status = STATUS_I2C_FAILED;
  //         }
  //         break;
  //     }
  //     case REQUEST_EXIT: {
  //         force_exit = true;
  //         break;
  //     }
  //     default : {
  //         resp_msg.req_type = REQUEST_UNKNOWN;
  //         resp_msg.req_status = STATUS_RESEND;
  //         break;
  //     }
  // }

  // stop thread thermal camera
  cam.stop();
}

int main(int argc, char* argv[]) {
  //    QApplication a(argc, argv);
  //    MainWindow w;
  //    w.show();
  //    return a.exec();
  std::thread t(loop_sensor);  // Separate thread for loop.

  // Wait for input character (this will suspend the main thread, but the loop
  // thread will keep running).
  std::cin.get();

  // Set the atomic boolean to true. The loop thread will exit from
  // loop and terminate.
  stop = true;

  t.join();

  return 0;
}
