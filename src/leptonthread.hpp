#ifndef LEPTONTHREAD_HPP
#define LEPTONTHREAD_HPP

#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QThread>
#include <QtCore>
#include <cstdint>
#include <ctime>
#include "camerathread.hpp"

// constant Lepton packet and frame
constexpr int PACKET_SIZE{164};
constexpr int PACKET_SIZE_UINT16{PACKET_SIZE / 2};
constexpr int PACKETS_PER_FRAME{60};
constexpr int FRAME_SIZE_UINT16{PACKET_SIZE_UINT16 * PACKETS_PER_FRAME};

// Lepton communication timing parameters
constexpr uint16_t MaxResetsPerSegment{750};  // packet resets
constexpr uint16_t MaxResetsPerFrame{30};     // segment resets
constexpr uint16_t MaxResetsBeforeReboot{2};  // frame resets
constexpr uint32_t LeptonLoadTime{2000};      // 0.002 s = 2 ms = 2000 us
constexpr uint32_t LeptonResetTime{1000};     // 0.001 s = 1 ms = 3000 us
constexpr uint32_t LeptonRebootTime{750000};  // 0.75 s = 750 ms = 750000 us

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
  ~LeptonThread() override;

  /**
   * @brief function that captures from various sources to update the UI
   *
   * This method is executed in a thread where the buffer from the thermal
   * camera is acquired, calculates the minimum and maximum value of the frames
   * and reproduces a colored map. Capture from the RGB camera. Overlays the two
   * source images in a single image. issues three signals to update the UI.
   *
   */
  void run() override;
  const int* colorMap;  // associate colors to the frame
 public slots:
  /**
   * @brief execute FFC operation
   *
   */
  void performFFC();

  /**
   * @brief changes the colour palette associated with the thermal image.
   *
   * @param colour[in] palette colour
   */
  void changeColourMap(const int* colour);

  /**
   * @brief capture a frame and save it to disk.
   *
   */
  void snapImage();

  /**
   * @brief Set the Mode object during overlaping image.
   *
   */
  void setMode(int mode);

 signals:
  void updateText(QString);
  void updateImage(QImage);
  void updateCam(QImage);
  void updateOverlay(QImage);

 protected:
  /**
   * @brief function that superimposes two images with different methods.
   *
   * This method accepts two images as input, and allows them to be overlaid by
   * different methods in a single image. Finally the UI is updated by issuing a
   * signal.
   *
   * @param thermal image containing thermal information
   * @param rgb image containing RGB information
   */
  inline void recalculateResult(const QImage& thermal, const QImage& rgb);

 private:
  QImage m_ir_image;  // thermal image
  QImage m_result;    // image containing the final result
  // buffer
  alignas(32) uint8_t result[PACKET_SIZE * PACKETS_PER_FRAME];
  alignas(32) uint16_t* m_frameBuffer;  // store actual frame
  CameraColour* cam;                    // interface raspicam
  QPainter::CompositionMode m_mode;     // composition mode
};

#endif  // LEPTONTHREAD_HPP
