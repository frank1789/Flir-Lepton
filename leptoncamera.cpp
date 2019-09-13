#include "leptoncamera.h"
#include <iostream>
#include "leptonutils.h"
#include "logger.h"

LeptonCamera::LeptonCamera()
    : grabber_thread_(),
    run_thread_{false},
    m_has_frame{false},
    m_sensor(),
    m_temperature{0.0}{
#if LOGGER
  LOG(INFO, "ctor lepton camera")
  LOG(INFO, "allocating buffer I/O frame")
  LOG(DEBUG, "init sensor temperature %lf", m_temperature)
#endif
  // Open communication with the sensor
  if (!m_sensor.open_connection()) {
#if LOGGER
    LOG(ERROR, "sensor lepton open connection")
#endif
    std::cerr << "Unable to open communication with the sensor" << std::endl;
    throw std::runtime_error("Connection failed.");
  }
  // Check lepton type
  m_lepton_type = m_sensor.getInfo_sensor();
  if (LeptonSensorType::UNKNOWN == m_lepton_type) {
#if LOGGER
    LOG(FATAL, "sensor lepton not recognized")
#endif
    throw std::runtime_error("Unknown lepton type.");
  }
// Prepare buffers
  m_config = LeptonCameraConfig(m_lepton_type);
  m_frame_read.resize(m_config.width * m_config.height);
  m_frame_write.resize(m_config.width * m_config.height);
};

LeptonCamera::~LeptonCamera() {
// Stop thread
#if LOGGER
  LOG(INFO, "dtor sensor")
#endif
  // Close communication with the sensor
  if (!m_sensor.close_connection()) {
#if LOGGER
    LOG(FATAL, "failed to close connection")
#endif
    std::cerr << "Unable to close communication with the sensor" << std::endl;
  }
};

void LeptonCamera::start() {
    // Avoid starting the thread if already runs
    if (false == run_thread_) {
        run_thread_ = true;
        grabber_thread_ = std::thread(&LeptonCamera::run, this);
    }
}

void LeptonCamera::stop() {
    // Stop the thread only if there is a thread running
    if (true == run_thread_) {
        run_thread_ = false;
        if (grabber_thread_.joinable()) {
            grabber_thread_.join();
        }
    }
}

void LeptonCamera::run() {
  while (run_thread_) {
#if LOGGER
    LOG(INFO, "enter loop 'run' for new frame")
#endif
    // Get new frame
    try {
        m_temperature = leptonI2C_InternalTemp();
#if LOGGER
      LOG(INFO, "try, get new frame")
      LOG(DEBUG, "sensor temperature: %3.2lf K", (m_temperature /100))
#endif
      if (!m_sensor.getFrame(m_frame_write.data(), LeptonFrameType::FRAME_U16)) {
        continue;
      }
    } catch (...) {
#if LOGGER
      LOG(WARN, "enter catch section, then reboot sensor")
#endif
      m_sensor.reboot_sensor();
      continue;
    }
// Lock resources and swap buffers
    lock_.lock();
    std::swap(m_frame_write, m_frame_read);
    m_has_frame = true;
    lock_.unlock();
  }
}


