#include "leptoncamera.h"
#include <iostream>
#include "leptonutils.h"
#include "logger.h"

LeptonCamera::LeptonCamera()
    : m_sensor(),
      m_sensor_temperature{0.0},
      m_has_frame{false},
      grabber_thread_(),
      run_thread_{false} {
#if LOGGER
  LOG(INFO, "ctor lepton camera")
  LOG(DEBUG, "init sensor temperature %lf", m_sensor_temperature)
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
  m_lepton_sensor = m_sensor.getInfo_sensor();
  if (LeptonSensor_t::UNKNOWN == m_lepton_sensor) {
#if LOGGER
    LOG(FATAL, "sensor lepton not recognized")
#endif
    throw std::runtime_error("Unknown lepton type.");
  }
// Prepare buffers
#if LOGGER
  LOG(INFO, "allocating buffer I/O frame")
#endif
  m_lepton_config = LeptonCameraConfig(m_lepton_sensor);
  m_frame_read.resize(m_lepton_config.width * m_lepton_config.height);
  m_frame_write.resize(m_lepton_config.width * m_lepton_config.height);
};

LeptonCamera::~LeptonCamera() {
// Stop thread
#if LOGGER
  LOG(INFO, "dtor sensor")
#endif
  stop();
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
#if LOGGER
    LOG(INFO, "start thread")
#endif
    run_thread_ = true;
    grabber_thread_ = std::thread(&LeptonCamera::run, this);
  }
}

void LeptonCamera::stop() {
// Stop the thread only if there is a thread running
#if LOGGER
  LOG(INFO, "stop thread")
#endif
  if (true == run_thread_) {
    run_thread_ = false;
    if (grabber_thread_.joinable()) {
      grabber_thread_.join();
#if LOGGER
      LOG(INFO, "success join thread")
#endif
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
#if LOGGER
      LOG(INFO, "try, get new frame")
#endif
      m_sensor_temperature = leptonI2C_InternalTemp();
#if LOGGER
      LOG(DEBUG, "sensor temperature: %lf", m_sensor_temperature)
#endif
      if (!m_sensor.getFrame(m_frame_write.data(), LeptonFrame_t::FRAME_U16)) {
        continue;
      }
    } catch (...) {
#if LOGGER
      LOG(WARN, "enter catch section")
#endif
      m_sensor.reboot_sensor();
#if LOGGER
      LOG(WARN, "reboot sensor")
#endif
      continue;
    }
// Lock resources and swap buffers
#if LOGGER
    LOG(DEBUG, "function 'run': lock resources")
#endif
    lock_.lock();
    std::swap(m_frame_write, m_frame_read);
    m_has_frame = true;
#if LOGGER
    LOG(INFO, "new frame")
#endif
    lock_.unlock();
#if LOGGER
    LOG(DEBUG, "function 'run': release resources")
#endif
  }
}

void LeptonCamera::getFrameU8(std::vector<uint8_t>& frame) {
#if LOGGER
  LOG(INFO, "enter getFrameU8")
#endif
  // Resize output frame
  frame.resize(m_frame_read.size());
  // Lock resources
  lock_.lock();
  // Find frame min and max
  uint16_t minValue = 65535;
  uint16_t maxValue = 0;
  for (size_t i = 0; i < m_frame_read.size(); i++) {
    if (m_frame_read[i] > maxValue) {
      maxValue = m_frame_read[i];
    }
    if (m_frame_read[i] < minValue) {
      minValue = m_frame_read[i];
    }
  }
  // Scale frame range and copy to output
  float scale = 255.f / static_cast<float>(maxValue - minValue);
  for (size_t i = 0; i < m_frame_read.size(); i++) {
    frame[i] = static_cast<uint8_t>((m_frame_read[i] - minValue) * scale);
  }
  m_has_frame = false;
  emit updateImageValue(maxValue, minValue);
#if LOGGER
  LOG(TRACE, "minValue %d, maxValue %d", minValue, maxValue)
  LOG(TRACE, "emit signal maxValue %d,\tminValue %d", maxValue, minValue)
#endif
  // Release resources
  lock_.unlock();
}

void LeptonCamera::getFrameU16(std::vector<uint16_t>& frame) {
  // Lock resources
  lock_.lock();
  std::copy(m_frame_read.begin(), m_frame_read.end(), frame.begin());
  m_has_frame = false;
  // Release resources
  lock_.unlock();
}

bool LeptonCamera::sendCommand(LeptonI2CCmd cmd, void* buffer) {
  return m_sensor.send_command(cmd, buffer);
}

std::ostream& operator<<(std::ostream& os, const LeptonSensor_t& lp) {
  switch (lp) {
    case LeptonSensor_t::UNKNOWN:
      os << "UNKNOW";
      break;
    case LeptonSensor_t::LEPTON2:
      os << "LEPTON2";
      break;
    case LeptonSensor_t::LEPTON3:
      os << "LEPTON3";
      break;
    default:
      os.setstate(std::ios_base::failbit);
  }
  return os;
}
