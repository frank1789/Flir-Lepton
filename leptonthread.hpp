#ifndef LEPTONTHREAD_HPP
#define LEPTONTHREAD_HPP

#include <cstdint>
#include <ctime>

#include <QImage>
#include <QPixmap>
#include <QThread>
#include <QtCore>

constexpr int PACKET_SIZE{164};
constexpr int PACKET_SIZE_UINT16{PACKET_SIZE / 2};
constexpr int PACKETS_PER_FRAME{60};
constexpr int FRAME_SIZE_UINT16{PACKET_SIZE_UINT16 * PACKETS_PER_FRAME};

class LeptonThread : public QThread {
  Q_OBJECT;

 public:
  LeptonThread();
  ~LeptonThread();

  void run();
  const int* colorMap;
 public slots:
  void performFFC();
  void rainMap();
  void greyMap();
  void ironMap();
  void snapImage();

 signals:
  void updateText(QString);
  void updateImage(QImage);

 private:
  // image
  QImage myImage;
  // buffer
  uint8_t result[PACKET_SIZE * PACKETS_PER_FRAME];
  uint16_t* frameBuffer;
};

#endif  // LEPTONTHREAD_HPP
