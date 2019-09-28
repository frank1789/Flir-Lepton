#ifndef IMAGECOMPOSERTHREAD_HPP
#define IMAGECOMPOSERTHREAD_HPP

#include <QImage>
#include <QPainter>
#include <QThread>

class ImageComposerThread : public QThread {
  Q_OBJECT
 public:
  ImageComposerThread();
  ~ImageComposerThread() override;

  void run() override;

 protected:
  void recalculateResult();

 public slots:
  void setMode(int index);
  void setRGBImage(QImage img);
  void setThermalImage(QImage img);

 signals:
  void updateImage(QImage);

 private:
  QImage *m_rgb;
  QImage *m_thermal;
  QImage *m_result;
  QPainter::CompositionMode m_mode;
};

#endif  // IMAGECOMPOSERTHREAD_HPP
