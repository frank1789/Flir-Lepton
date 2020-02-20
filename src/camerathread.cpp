#include "camerathread.hpp"

#include <unistd.h>

#include <fstream>
#include <iostream>

#include "log/instrumentor.h"
#include "log/logger.h"

CameraColour::CameraColour() {
  auto size = camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB);
  m_buffer = std::make_unique<unsigned char[]>(size);
#if LOGGER
  LOG(INFO, "ctor CameraColour")
#endif
  // Open the camera
  if (!camera.open()) {
#if LOGGER
    LOG(ERROR, "unable open camera throw runtime error.")
#endif
    std::cerr << "Unable to open camera";
    throw std::runtime_error("Starting camera failed");
  }
  usleep(RaspicamLoadTime);
}

CameraColour::~CameraColour() {
  // close camera
#if LOGGER
  LOG(INFO, "dtor CameraColour")
#endif
}

QImage CameraColour::getImageRGB() {
  PROFILE_FUNCTION();
  // capture
  camera.grab();
  camera.retrieve(m_buffer.get(), raspicam::RASPICAM_FORMAT_IGNORE);
  // convert the data and send to the caller to handle
  auto width = camera.getWidth();
  auto height = camera.getHeight();
  QImage image = QImage(m_buffer.get(), width, height, QImage::Format_RGB888)
                     .scaled(1024, 768, Qt::KeepAspectRatio);
#if LOGGER
  LOG(DEBUG, "FPS camera: %u", camera.getFrameRate())
#endif
  return image;
}
