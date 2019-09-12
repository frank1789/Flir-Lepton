#ifndef LEPTON_SENSOR_H
#define LEPTON_SENSOR_H

#include <atomic>
#include <cstdint>
#include <map>
#include <vector>



enum class LeptonSensor_t { UNKNOWN, LEPTON2, LEPTON3 };

const std::map<int, LeptonSensor_t> MapLepton{{2, LeptonSensor_t::LEPTON2},
                                              {3, LeptonSensor_t::LEPTON3}};

enum class LeptonFrame_t { FRAME_U8, FRAME_U16 };

// Lepton I2C commands
enum class LeptonI2CCmd {
  RESET,          // Sensor connection reset
  REBOOT,         // Sensor reboot
  FFC,            // Flat Field Correction
  SENSOR_TEMP_K,  // Get sensor internal temperature in kelvin
  SHUTTER_OPEN,   // Open camera shutter
  SHUTTER_CLOSE,  // Close camera shutter
  VOID            // No command (just for frame request msg)
};

// Lepton communication timing parameters
constexpr uint16_t MaxResetsPerSegment{500};   // packet resets
constexpr uint16_t MaxResetsPerFrame{40};      // segment resets
constexpr uint16_t MaxResetsBeforeReboot{2};   // frame resets
constexpr uint32_t LeptonLoadTime{200000};     // 0.2 s = 200 ms = 200000 us
constexpr uint32_t LeptonResetTime{300000};    // 0.3 s = 300 ms = 300000 us
constexpr uint32_t LeptonRebootTime{1500000};  // 1.5 s = 1500 ms = 1500000 us

struct LeptonCameraConfig {
  uint16_t packet_size;          // SPI packet size in bytes
  uint16_t packet_size_uint16;   // SPI packet size in words
  uint16_t packets_per_segment;  // SPI packets per segment
  uint16_t packets_per_read;  // number of SPI packets read in one SPI read call

  uint32_t segment_size;                 // segment size in bytes
  uint32_t segment_size_uint16;          // segment size in words
  uint16_t segments_per_frame;           // number of segments per frame
  uint16_t segment_number_packet_index;  // packet index for the packet
                                         // containing the segment number
  uint32_t reset_wait_time;  // wait time in microseconds for each reset

  uint32_t spi_speed;  // SPI speed
  uint16_t width;      // Frame width
  uint16_t height;     // Frame height

  LeptonCameraConfig() = default;
  explicit LeptonCameraConfig(LeptonSensor_t lp);
};

class LeptonSensor {
 public:
  LeptonSensor() = default;
  LeptonSensor(LeptonSensor const &) = delete;
  LeptonSensor &operator=(const LeptonSensor &) = delete;
  ~LeptonSensor() = default;

  bool open_connection();
  bool close_connection();
  bool reset_connection();
  bool reset_SPI_connection();
  bool reboot_sensor();
  bool send_command(LeptonI2CCmd cmd, void *buffer);
  bool getFrame(void *frame, LeptonFrame_t type);
  LeptonSensor_t getInfo_sensor();
  friend std::ostream &operator<<(std::ostream &os, const LeptonSensor_t &lp);

 protected:
  /**
   * @brief Read 1 frame segment over SPI
   * @return Number of resets during a segment read
   */
  int LeptonReadSegment(const int max_resets, uint8_t *data_buffer);

  /**
   * @brief Read 1 frame over SPI
   * @return Number of resets during the complete frame read
   */
  int LeptonReadFrame();

  /**
   * @brief Tries to re-sync SPI communication with sensor
   * @param resetsToReboot  Number of resets until a reboot is required
   * @throw Runtime error if unable to re-sync with Lepton
   */
  void LeptonResync(uint16_t &resetsToReboot);

  /**
   * @brief Unpack latest received frame, keep IR full range
   */
  void LeptonUnpackFrame16(uint16_t *frame);

  /**
   * @brief Unpack latest received frame, and scale IR range to [0, 255], using
   *        frame's min and max IR values
   */
  void LeptonUnpackFrame8(uint8_t *frame);

 private:
  LeptonCameraConfig m_config;
  std::vector<uint16_t> m_frame_buffer;
  int m_count{0};
  int m_spi_port{0};
  std::atomic<bool> m_force_reboot;
};

std::ostream &operator<<(std::ostream &os, const LeptonSensor_t &lp);



#endif  // LEPTON_SENSOR_H
