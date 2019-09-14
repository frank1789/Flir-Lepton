#include <QFile>
#include <QImage>
#include <QPainter>
#include <QPixmap>

#include "thermalimage.h"
#include "leptoncamera.h"
#include "logger.h"
#include "palettes.h"

ThermalImage::ThermalImage(const LeptonCamera &camera)
    : m_rgbImage(camera.width(), camera.height(), QImage::Format_RGB888) {
#if LOGGER
  LOG(INFO, "ctor ThermalImage")
#endif
  m_frame_width = camera.width();
  m_frame_height = camera.height();
  // create the initial image
  QRgb red = qRgb(255, 0, 0);
  m_rgbImage.setPixel(m_frame_width, m_frame_height, red);
  // connect signals
  bool success = connect(&camera, &LeptonCamera::updateImageValue, this,
                         &ThermalImage::updateImageValue);
  Q_ASSERT(success);
  // emit signal to draw image
  emit updateImage(m_rgbImage);
}

void ThermalImage::updateImageValue(const unsigned int &max,
                                    const unsigned int &min) {
  this->rawMax = max;
  this->rawMin = min;
#if LOGGER
  LOG(DEBUG, "update value max %d, min %d", max, min)
#endif
}

ThermalImage::~ThermalImage() {}

 void ThermalImage::setImage(const std::vector<uint8_t> &imgU8) {
  rawdataU8 = QVector<uint8_t>::fromStdVector(imgU8);
#if LOGGER
  LOG(INFO, "fill from std::vector uint8_t mode")
#endif
}

 void ThermalImage::setImage(const std::vector<uint16_t> &imgU16) {
  rawdataU16 = QVector<uint16_t>::fromStdVector(imgU16);
#if LOGGER
  LOG(INFO, "fill from std::vector uint16_t mode")
#endif
}

 void ThermalImage::setImage(const QVector<uint8_t> &imgU8) {
     rawdataU8 = imgU8;
#if LOGGER
  LOG(INFO, "fill QVector uint8_t mode")
#endif
}

 void ThermalImage::setImage(const QVector<uint16_t> &imgU16) {
  rawdataU16 = imgU16;
#if LOGGER
  LOG(INFO, "fill QVector uint16_t mode")
#endif
}
