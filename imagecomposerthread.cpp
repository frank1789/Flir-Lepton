#include "imagecomposerthread.hpp"
#include "log/logger.h"
#include <QPainter>


ImageComposer::ImageComposer(QWidget *parent) : QThread()
{
  m_result  = new QImage(640, 480, QImage::Format_RGB888);
  m_source = new QImage(640, 480, QImage::Format_RGB888);
  m_destination = new QImage(640, 480, QImage::Format_RGBA8888_Premultiplied);

    #if LOGGER
  LOG(INFO, "ctor ImageCompose allocate")
  #endif
}

ImageComposer::~ImageComposer()
{
  delete m_result;
  delete m_source;
  delete m_destination;
  #if LOGGER
  LOG(INFO, "dtor ImageCompose deallocate")
  #endif
}

void ImageComposer::recalculate_result()
{
    QPainter::CompositionMode mode = QPainter::CompositionMode_Overlay;

    QPainter painter(m_result);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(m_result->rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, *m_destination);
    painter.setCompositionMode(mode);
    painter.drawImage(0, 0, *m_source);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.fillRect(m_result->rect(), Qt::white);
    painter.end();
      #if LOGGER
  LOG(DEBUG, "recalcuate_result to overlap to image")
  #endif

}

void ImageComposer::run() {
    #if LOGGER
    LOG(INFO, "run imagecomposer")
    #endif
    while(true) {
    this->recalculate_result();
    emit updateImage(*m_result);
    }
}

void ImageComposer::set_thermal_image(QImage img)
{
    *m_destination = img;
      #if LOGGER
  LOG(INFO, "update thermal image")
  #endif
}

void ImageComposer::set_rgb_image(QImage img)
{
    *m_source = img;
      #if LOGGER
  LOG(INFO, "update rgb image")
  #endif

}
