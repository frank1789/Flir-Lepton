#ifndef CAMERAWORKER_H
#define CAMERAWORKER_H

#include <raspicam/raspicam.h>
#include <QImage>
#include <QThread>

// constant raspberry Camera V2
constexpr unsigned int RaspicamLoadTime{30000};  // 0.03 s = 30 ms = 30000 us
constexpr unsigned int RaspicamResetTime{200};   // 0.0002 s = 0.2 ms = 200 us

/**
 * @brief Class control and get data from Raspberry Camera.
 *
 * This class grab data and converting it in QImage, then the image can be
 * displayed in QLabel. Depend on library
 * [Raspicam](https://github.com/cedricve/raspicam)
 */
class CameraThread : public QThread {
  Q_OBJECT
 public:
  /**
   * @brief Construct a new Camera Thread object.
   *
   * Open Camera connection, then, registers QImage as metatype and allocate
   * buffer at size equal RASPICAM_FORMAT_RGB image format.
   */
  CameraThread();

  /**
   * @brief Destroy the Camera Thread object.
   *
   * Deallocate buffer and close the Camera connection.
   */
  ~CameraThread();

  /**
   * @brief This method acquires images from the camera during operation and
   * emits the signal to send the required buffer.
   *
   */
  void run() override;
 signals:
  /**
   * @brief emits the image from the camera.
   *
   * This signal is emitted during run function when it grabs an image
   * from the camera and converts it to a QImage.
   * @param[in] image
   */
  void updateImage(QImage &image);

 private:
  raspicam::RaspiCam camera;  // Access the Raspberry Pi camera
  bool cameraRunning;         // status camera running
  unsigned char *m_buffer;  // raw data from camera, before converted in QImage
};

#endif  // CAMERAWORKER_H
