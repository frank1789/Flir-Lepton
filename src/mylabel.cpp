#include "mylabel.hpp"

#include <QColor>
#include <QPainter>

#include "instrumentor.h"
#include "logger.h"

MyLabel::MyLabel(QWidget *parent) : QLabel(parent) {}

MyLabel::~MyLabel() {}

// when the system calls setImage, we'll set the label's pixmap
void MyLabel::setImage(QImage image) {
  PROFILE_FUNCTION();
  QPixmap pixmap = QPixmap::fromImage(image);
  setPixmap(pixmap.scaled(this->width(), this->height(), Qt::KeepAspectRatio));
}
