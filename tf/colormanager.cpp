#include "colormanager.hpp"

#include <QColor>
#include <QPainter>
#include <QString>

#include "list_colour.hpp"

QColor ColorManager::getColor(QString element) {
  int index = elements.indexOf(element);

  if (index >= 0) return colors.at(index);

  QColor newColor = getNewColor();
  elements.append(element);
  colors.append(newColor);
  return newColor;
}

QColor ColorManager::getNewColor() {
  QColor color = def_color(defColors, elements.count());

  if (!rgb) {
    int r = color.red();
    int b = color.blue();

    color.setRed(b);
    color.setBlue(r);
  }

  return color;
}

bool ColorManager::getRgb() const { return rgb; }

void ColorManager::setRgb(bool value) { rgb = value; }

int getColor(QImage mask, QColor color, int x, int y) {
  return color == Qt::red ? qRed(mask.pixel(x, y))
                          : color == Qt::blue ? qBlue(mask.pixel(x, y))
                                              : qGreen(mask.pixel(x, y));
}

int billinerColor(QImage mask, QColor color, int xa, int xb, int xc, int xd,
                  int ya, int yb, int yc, int yd, double alpha, double beta) {
  int pa = getColor(mask, color, xa, ya);
  int pb = getColor(mask, color, xb, yb);
  int pc = getColor(mask, color, xc, yc);
  int pd = getColor(mask, color, xd, yd);
  auto result = (1 - alpha) * (1 - beta) * pa + alpha * (1 - beta) * pb +
                (1 - alpha) * beta * pc + alpha * beta * pd + 0.5;
  return static_cast<int>(result);
}

uint billinearPixel(QImage mask, double sx, double sy, int k, int j) {
  double alpha;
  double beta;

  int xa = static_cast<int>(k / sx);
  int ya = static_cast<int>(j / sy);
  int xb = xa + 1;
  int yb = ya;
  int xc = xa;
  int yc = ya + 1;
  int xd = xa + 1;
  int yd = ya + 1;
  if (xb >= mask.width()) xb--;
  if (xd >= mask.width()) xd--;
  if (yc >= mask.height()) yc--;
  if (yd >= mask.height()) yd--;
  alpha = k / sx - xa;
  beta = j / sy - ya;

  int red =
      billinerColor(mask, Qt::red, xa, xb, xc, xd, ya, yb, yc, yd, alpha, beta);
  int green = billinerColor(mask, Qt::green, xa, xb, xc, xd, ya, yb, yc, yd,
                            alpha, beta);
  int blue = billinerColor(mask, Qt::blue, xa, xb, xc, xd, ya, yb, yc, yd,
                           alpha, beta);

  return qRgb(red, green, blue);
}

QImage ColorManager::billinearInterpolation(QImage mask, double newHeight,
                                            double newWidth) {
  const double sy = newHeight / mask.height();
  const double sx = newWidth / mask.width();

  // Resize mask to box size
  auto res_w = static_cast<int>(mask.width() * sx);
  auto res_h = static_cast<int>(mask.height() * sy);
  QImage maskScaled(res_w, res_h, QImage::Format_ARGB32_Premultiplied);
  maskScaled.fill(Qt::transparent);

  // Billinear interpolation
  // https://chu24688.tian.yam.com/posts/44797337
  for (int j = 0; j < maskScaled.height(); j++)
    for (int k = 0; k < maskScaled.width(); k++)
      maskScaled.setPixel(k, j, billinearPixel(mask, sx, sy, k, j));

  return maskScaled;
}

QImage ColorManager::applyTransformation(QImage image,
                                         QTransform painterTransform) {
  QPainter painter;
  if (painter.begin(&image)) {
    painter.setTransform(painterTransform);
    painter.end();
  }
  return image;
}
