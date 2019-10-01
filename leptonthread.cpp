#include "leptonthread.hpp"
#include <unistd.h>
#include <QLabel>
#include "Lepton_I2C.h"
#include "SPI.h"
#include "log/logger.h"
#include "palettes.h"

LeptonThread::LeptonThread()
    : QThread(),
      colorMap(colormap::ironblack),
      m_result(640, 480, QImage::Format_ARGB32_Premultiplied) {
  cam = new CameraColour();
  m_mode = QPainter::CompositionMode_Overlay;
#if LOGGER
  LOG(INFO, "ctor LeptonThread")
#endif
}

LeptonThread::~LeptonThread() {
  if (cam) {
    delete cam;
  }
}

void LeptonThread::run() {
  // generate thermal initial image
  m_ir_image = QImage(80, 60, QImage::Format_RGB888);
  // open SPI port
  leptonSPI_OpenPort(0);
  usleep(LeptonLoadTime);
  while (true) {
    // read data packets from lepton over SPI
    int resets = 0;
    for (int j = 0; j < PACKETS_PER_FRAME; j++) {
      // if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again
      // loop
      read(spi_cs0_fd, result + sizeof(uint8_t) * PACKET_SIZE * j,
           sizeof(uint8_t) * PACKET_SIZE);
      int packetNumber = result[j * PACKET_SIZE + 1];
      if (packetNumber != j) {
        j = -1;
        resets += 1;
        usleep(LeptonResetTime);
        if (resets == MaxResetsPerSegment) {
          leptonSPI_ClosePort(0);
          usleep(LeptonRebootTime);
          leptonSPI_OpenPort(0);
        }
      }
    }
#if LOGGER
    if (resets >= MaxResetsPerFrame) {
      LOG(DEBUG, "reading, resets: %d", resets)
    }
#endif

    m_frameBuffer = (uint16_t *)result;
    int row, column;
    uint16_t value;
    uint16_t minValue = 65535;
    uint16_t maxValue = 0;

    for (int i = 0; i < FRAME_SIZE_UINT16; i++) {
      // skip the first 2 uint16_t's of every packet, they're 4 header bytes
      if (i % PACKET_SIZE_UINT16 < 2) {
        continue;
      }

      // flip the MSB and LSB at the last second
      int temp = result[i * 2];
      result[i * 2] = result[i * 2 + 1];
      result[i * 2 + 1] = temp;
      // find value min/max for proper scale
      value = m_frameBuffer[i];
      if (value > maxValue) {
        maxValue = value;
      }
      if (value < minValue) {
        minValue = value;
      }
      column = i % PACKET_SIZE_UINT16 - 2;
      row = i / PACKET_SIZE_UINT16;
    }

    float diff = maxValue - minValue;
    float scale = 255 / diff;
    QRgb color;
    for (int i = 0; i < FRAME_SIZE_UINT16; i++) {
      if (i % PACKET_SIZE_UINT16 < 2) {
        continue;
      }
      value = (m_frameBuffer[i] - minValue) * scale;
      color = qRgb(this->colorMap[3 * value], this->colorMap[3 * value + 1],
                   this->colorMap[3 * value + 2]);
      column = (i % PACKET_SIZE_UINT16) - 2;
      row = i / PACKET_SIZE_UINT16;
      m_ir_image.setPixel(column, row, color);
    }
    QImage colour_image = cam->getImageRGB();
    emit updateImage(m_ir_image);
    emit updateCam(colour_image);
    recalculateResult(m_ir_image.scaled(640, 480, Qt::KeepAspectRatio),
                      colour_image.scaled(640, 480, Qt::KeepAspectRatio));
  }
  //  close SPI port
  leptonSPI_ClosePort(0);
}
void LeptonThread::snapImage() {
  QImage bigger = m_ir_image.scaled(6 * m_ir_image.size().width(),
                                    6 * m_ir_image.size().height());
  bigger.save("/home/pi/snap.png");
  QLabel *popup = new QLabel();
  QPixmap pixmap = QPixmap::fromImage(bigger);
  popup->setPixmap(pixmap);
  popup->show();
}

void LeptonThread::performFFC() {
  // perform FFC
  lepton_perform_ffc();
}

void LeptonThread::changeColourMap(const int *colour) {
  this->colorMap = colour;
  this->start();
}

void LeptonThread::setMode(int mode) {
  m_mode = static_cast<QPainter::CompositionMode>(mode);
}

void LeptonThread::recalculateResult(const QImage &thermal, const QImage &rgb) {
  QPainter painter(&m_result);
  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.fillRect(m_result.rect(), Qt::transparent);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.drawImage(0, 0, thermal);
  painter.setCompositionMode(m_mode);
  painter.drawImage(0, 0, rgb);
  painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
  painter.fillRect(m_result.rect(), Qt::white);
  painter.end();
  emit updateOverlay(m_result);
}
