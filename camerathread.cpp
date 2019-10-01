#include "camerathread.hpp"
#include <unistd.h>
#include <fstream>
#include <iostream>
#include "common.hpp"
#include "log/logger.h"

CameraColour::CameraColour() {
  auto size = camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB);
  m_buffer = std::make_unique<unsigned char[]>(size);

#if LOGGER
  LOG(INFO, "ctor CameraColour")
#endif
  // Open the camera
  if (!camera.open()) {
    std::cerr << "Unable to open camera";
    throw std::runtime_error("Starting camera failed");
  }
  usleep(RaspicamLoadTime);
}

CameraColour::~CameraColour() {
  // close camera
  camera.release();
#if LOGGER
  LOG(INFO, "dtor CameraColour")
#endif
}

QImage CameraColour::getImageRGB() {
  // capture
  camera.grab();
  camera.retrieve(m_buffer.get(), raspicam::RASPICAM_FORMAT_IGNORE);
  // convert the data and send to the caller to handle
  QImage image = QImage(m_buffer.get(), camera.getWidth(), camera.getHeight(),
                        QImage::Format_RGB888);
  return image;
}
