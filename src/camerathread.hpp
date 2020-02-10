#ifndef CAMERAWORKER_H
#define CAMERAWORKER_H

#include <raspicam/raspicam.h>

#include <QImage>
#include <memory>

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
class CameraColour {
 public:
  /**
   * @brief Construct a new Camera Thread object.
   *
   * Open Camera connection, then, registers QImage as metatype and allocate
   * buffer at size equal RASPICAM_FORMAT_RGB image format.
   */
  CameraColour();

  /**
   * @brief Destroy the Camera Thread object.
   *
   * Deallocate buffer and close the Camera connection.
   */
  ~CameraColour();

  /**
   * @brief This method acquires images from the camera during operation.
   *
   * @return a frame
   */
  QImage getImageRGB();

 private:
  raspicam::RaspiCam camera;  // Access the Raspberry Pi camera
  // raw data from camera, before converted in QImage
  std::unique_ptr<unsigned char[]> m_buffer;
};

#endif  // CAMERAWORKER_H
