#include "thermalimage.h"
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include "thermalimage.h"
#include "leptoncamera.h"
#include "logger.h"
#include "palettes.h"

ThermalImage::ThermalImage(const LeptonCamera &camera)
    : m_index_image(0),
      m_rgbImage(camera.width(), camera.height(), QImage::Format_RGB888) {
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
  LOG(INFO, "fill QVector uint8_t mode")
#endif
}

 void ThermalImage::setImage(const std::vector<uint16_t> &imgU16) {
  rawdataU16 = QVector<uint16_t>::fromStdVector(imgU16);
#if LOGGER
  LOG(INFO, "fill QVector uint16_t mode")
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

// void ThermalImage::save_raw_file() {
//#if LOGGER
//  LOG(INFO, "save raw file and colour image")
//  LOG(TRACE, "slot update maxValue %d;\tminValue %d", rawMax, rawMin)
//#endif
//  // Raw file format: binary, one word for min value, one for max value, then
//  // 80*60 words of raw data
//  QFile rawFile(QString("raw_%1.bin").arg(m_index_image));
//  rawFile.open(QIODevice::Truncate | QIODevice::ReadWrite);
//  QDataStream rawOut(&rawFile);
//  rawOut << rawMin << rawMax;
//  rawOut.writeRawData((char *)&rawdataU8[0], 2 * rawdataU8.size());
//  rawFile.close();

//  // JPG image, top quality
//  rgbImage.save(QString("IMG_%1.jpg").arg(m_index_image), "JPG", 100);
//  ++m_index_image;
//#if LOGGER
//  LOG(DEBUG, "update index image: %d", m_index_image)
//#endif
//}

//// Map "rawData" to rgb values in "rgbImage" via the colormap
// void ThermalImage::map_raw_data_to_rgb() {
//  auto diff = rawMax - rawMin + 1;
//  for (uint32_t y = 0; y < m_frame_height; ++y) {
//    for (uint32_t x = 0; x < m_frame_width; ++x) {
//      auto baseValue =
//          rawdataU8[m_frame_width * y + x];  // take input value in [0, 65536)
//      auto scaledValue = 256 * (baseValue - rawMin) /
//                         diff;  // map value to interval [0, 256), and set the
//                                // pixel to its color value above
//      rgbImage.setPixel(x, y,
//                        qRgb(colormap::grayscale[3 * scaledValue],
//                             colormap::grayscale[3 * scaledValue + 1],
//                             colormap::grayscale[3 * scaledValue + 2]));
//    }
//  }
//}

// void ThermalImage::save_pgm_file() {
//  char image_name[32];
//  do {
//    sprintf(image_name, "IMG_%.4lu.pgm", m_index_image);
//    ++m_index_image;
//    if (m_index_image > 99999) {
//      m_index_image = 0;
//      break;
//    }
//  } while (access(image_name, F_OK) == 0);
//  // open file
//  FILE *f = fopen(image_name, "w");
//  if (f == nullptr) {
//    std::cerr << "Error opening file! << \n";
//    exit(1);
//  }
////   printf("Calculating min/max values for proper scaling...\n");
////   for (i = 0; i < 60; i++) {
////     for (j = 0; j < 80; j++) {
////       if (lepton_image[i][j] > maxval) {
////         maxval = lepton_image[i][j];
////       }
////       if (lepton_image[i][j] < minval) {
////         minval = lepton_image[i][j];
////       }
////     }
////   }
//#if LOGGER
//  LOG(TRACE, "maxval = %u", rawMax)
//  LOG(TRACE, "minval = %u", rawMin)
//#endif
//  fprintf(f, "P2\n80 60\n%u\n", rawMax - rawMin);
//  for (uint32_t y = 0; y < m_frame_height; ++y) {
//    for (uint32_t x = 0; x < m_frame_width; ++x) {
////      fprintf(f, "%d", rawdataU8[m_frame_width * y + x]);
//                 fprintf(f, "%d ", rawdataU8[m_frame_width * y + x] - rawMin);
//    }

//    //   for (i = 0; i < 60; i++) {
//    //     for (j = 0; j < 80; j++) {
//    //       fprintf(f, "%d ", lepton_image[i][j] - minval);
//    //     }
//    fprintf(f, "\n");
//  }
//  fprintf(f, "\n\n");

//  fclose(f);
//}
