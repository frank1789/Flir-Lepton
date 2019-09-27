#ifndef LEPTONTHREAD_HPP
#define LEPTONTHREAD_HPP

#include <QImage>
#include <QPixmap>
#include <QThread>
#include <QtCore>
#include <cstdint>
#include <ctime>

// constant Lepton packet and frame
constexpr int PACKET_SIZE{164};
constexpr int PACKET_SIZE_UINT16{PACKET_SIZE / 2};
constexpr int PACKETS_PER_FRAME{60};
constexpr int FRAME_SIZE_UINT16{PACKET_SIZE_UINT16 * PACKETS_PER_FRAME};

// Lepton communication timing parameters
constexpr uint16_t MaxResetsPerSegment{750};   // packet resets
constexpr uint16_t MaxResetsPerFrame{30};     // segment resets
constexpr uint16_t MaxResetsBeforeReboot{2};  // frame resets
constexpr uint32_t LeptonLoadTime{2000};       // 0.002 s = 2 ms = 2000 us
constexpr uint32_t LeptonResetTime{1000};      // 0.001 s = 1 ms = 3000 us
constexpr uint32_t LeptonRebootTime{750000};   // 0.75 s = 750 ms = 750000 us

/**
 * @brief Class control Flir Lepton Camera 2.5
 *
 * This class is responsible for grabing data from IR sensor
 * and converting it in to QImage displayed in Mainwindows
 *
 */
class LeptonThread : public QThread {
  Q_OBJECT

 public:
  /**
   * @brief Construct a new Lepton Thread object
   *
   */
  LeptonThread();

  /**
   * @brief Destroy the Lepton Thread object
   *
   */
  ~LeptonThread();

  /**
   * @brief
   *
   */
  void run() override;
  const int* colorMap;  // associate colors to the frame
 public slots:
  void performFFC();
  void changeColourMap(const int* colour);
  void snapImage();

 signals:
  void updateText(QString);
  void updateImage(QImage);

 private:
  QImage m_ir_image;  // image
  // buffer
  uint8_t result[PACKET_SIZE * PACKETS_PER_FRAME];
  uint16_t* m_frameBuffer;
};

#endif  // LEPTONTHREAD_HPP
