#ifndef LEPTONCAMERA_H
#define LEPTONCAMERA_H

#include <QObject>
#include <mutex>
#include <thread>
#include <vector>
#include "leptonsensor.h"

class LeptonCamera : public QObject {
  Q_OBJECT
 public:
  LeptonCamera();
  ~LeptonCamera();
  // Delete copy constructor and copy operator
  LeptonCamera(LeptonCamera const &) = delete;
  LeptonCamera &operator=(LeptonCamera const &) = delete;

  /**
   * @brief Start camera grabber thread
   */
  void start();

  /**
   * @brief Stop camera grabber thread
   */
  void stop();

  /**
   * @brief Sends an I2C command to the Lepton sensor
   * @param cmd     I2C known command, see LeptonCommon.h
   * @param buffer  Data buffer, some I2C commands return data (buffer should
   *                have proper size to fit all the returned data)
   * @return true, if I2C command succeed, false otherwise
   */
  bool sendCommand(LeptonI2CCmd cmd, void *buffer);

  /**
   * @brief Lepton frame accessors
   */
  inline bool hasFrame() const { return m_has_frame; }
  void getFrameU8(std::vector<uint8_t> &frame);
  void getFrameU16(std::vector<uint16_t> &frame);

  /**
   * @brief Lepton sensor specification accessors
   */
  inline double SensorTemperature() const { return m_sensor_temperature; }
  inline LeptonSensor_t LeptonVersion() const { return m_lepton_sensor; }
  inline uint32_t width() const { return m_lepton_config.width; }
  inline uint32_t height() const { return m_lepton_config.height; }
  friend std::ostream &operator<<(std::ostream &os, const LeptonSensor_t &lp);

 signals:
  void updateImageValue(const uint16_t max, const uint16_t min);

 private:
  /**
   * @brief Camera grabber (runs in a parallel thread)
   */
  void run();
  // sensor info
  LeptonSensor m_sensor;
  LeptonSensor_t m_lepton_sensor;
  LeptonCameraConfig m_lepton_config;
  double m_sensor_temperature;

  // InfraRed double buffer
  std::vector<uint16_t> m_frame_read;
  std::vector<uint16_t> m_frame_write;
  std::atomic<bool> m_has_frame;

  // thread camera management
  std::thread grabber_thread_;
  std::mutex lock_;
  std::atomic<bool> run_thread_;
};

std::ostream &operator<<(std::ostream &os, const LeptonSensor_t &lp);

#endif  // LEPTONCAMERA_H
