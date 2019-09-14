#include <unistd.h>

#include <QLabel>

#include "Lepton_I2C.h"
#include "SPI.h"
#include "leptonthread.hpp"
#include "log/logger.h"
#include "palettes.h"

LeptonThread::LeptonThread() : QThread(), colorMap(colormap::ironblack) {
  m_temperature = new Temperature();
}

LeptonThread::~LeptonThread() {
  if (m_temperature) {
    delete m_temperature;
  }
}

void LeptonThread::run() {
  // generate initial image
  myImage = QImage(80, 60, QImage::Format_RGB888);
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
    if (resets >= kMaxResetsPerFrame) {
      LOG(DEBUG, "reading, resets: %d", resets)
    }
#endif
    // acquire temperature
    m_temperature->read_from_sensor();
    frameBuffer = (uint16_t *)result;
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
      value = frameBuffer[i];
      if (value > maxValue) {
        maxValue = value;
      }
      if (value < minValue) {
        minValue = value;
      }
      column = i % PACKET_SIZE_UINT16 - 2;
      row = i / PACKET_SIZE_UINT16;
    }
    // comute temperature
    m_temperature->frame_to_temperature(maxValue, minValue);
    float diff = maxValue - minValue;
    float scale = 255 / diff;
    QRgb color;
    for (int i = 0; i < FRAME_SIZE_UINT16; i++) {
      if (i % PACKET_SIZE_UINT16 < 2) {
        continue;
      }
      value = (frameBuffer[i] - minValue) * scale;
      color = qRgb(this->colorMap[3 * value], this->colorMap[3 * value + 1],
                   this->colorMap[3 * value + 2]);
      column = (i % PACKET_SIZE_UINT16) - 2;
      row = i / PACKET_SIZE_UINT16;
      myImage.setPixel(column, row, color);
    }
    // lets emit the signal for update
    emit updateImage(myImage);
  }
  leptonSPI_ClosePort(0);
}
void LeptonThread::snapImage() {
  QImage bigger =
      myImage.scaled(6 * myImage.size().width(), 6 * myImage.size().height());
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
void LeptonThread::rainMap() { this->colorMap = colormap::rainbow; }
void LeptonThread::greyMap() { this->colorMap = colormap::grayscale; }
void LeptonThread::ironMap() { this->colorMap = colormap::ironblack; }
