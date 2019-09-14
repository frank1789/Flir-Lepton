#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <QLabel>
#include <QWidget>

class Image : public QLabel {
  Q_OBJECT
 public:
  Image(QWidget *parent = nullptr);
  ~Image();

 public slots:
  void setImage(QImage);
};

#endif  // IMAGE_HPP
