#include "image.hpp"

Image::Image(QWidget *parent) : QLabel(parent) {}
Image::~Image() {}

// when the system calls setImage, we'll set the label's pixmap
void Image::setImage(QImage image) {
  QPixmap pixmap = QPixmap::fromImage(image);
  int w = this->width();
  int h = this->height();
  setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
}
