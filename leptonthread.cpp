#include "leptonthread.hpp"
#include "logger.h"

LeptonThread::LeptonThread() : QThread(), m_image(m_camera) {
    m_frameU8 = new QVector<uint8_t>(m_camera.width() * m_camera.height());
}

LeptonThread::~LeptonThread() {
    m_camera.stop();
    delete m_frameU8;
#if LOGGER
    LOG(INFO, "dtor LeptonThread")
#endif
}

void LeptonThread::run() {
  m_camera.start();
  auto temperature = m_camera.SensorTemperature();
#if LOGGER
  LOG(DEBUG, "temperature acquired: %lf", temperature)
#endif
  while (true) {
       m_camera.getFrameU8(*m_frameU8);
       m_image.setImage(*m_frameU8);
  }
}
