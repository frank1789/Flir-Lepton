#ifndef COLORMANAGER_HPP
#define COLORMANAGER_HPP

#include <QObject>

QT_BEGIN_NAMESPACE
class QColor;
class QString;
class QImage;
QT_END_NAMESPACE

class ColorManager {
 public:
  explicit ColorManager() = default;
  QColor getColor(QString element);
  QColor getColor(int i) const;
  static QImage billinearInterpolation(QImage mask, double newHeight,
                                       double newWidth);
  static QImage applyTransformation(QImage image, QTransform painterTransform);
  bool getRgb() const;
  void setRgb(bool value);

 private:
  QStringList elements;
  QList<QColor> colors;
  QColor getNewColor();
  bool rgb = true;
};

#endif  // COLORMANAGER_HPP
