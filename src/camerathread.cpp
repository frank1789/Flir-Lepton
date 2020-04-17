#include "camerathread.hpp"

#include <unistd.h>

#include <fstream>
#include <iostream>

#include "instrumentor.h"
#include "logger.h"

CameraColour::CameraColour() {
  auto size = camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB);
  m_buffer = std::make_unique<unsigned char[]>(size);
  LOG(LevelAlert::I, "ctor CameraColour")
  // Open the camera
  if (!camera.open()) {
    LOG(LevelAlert::E, "unable open camera throw runtime error.")
    std::cerr << "Unable to open camera";
    throw std::runtime_error("Starting camera failed");
  }
  usleep(RaspicamLoadTime);
}

CameraColour::~CameraColour(){LOG(LevelAlert::I, "dtor CameraColour")}

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
  LOG(LevelAlert::D, "FPS camera: %u", camera.getFrameRate())
  return image;
}
