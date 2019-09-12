#include <unistd.h>
#include <ctime>
#include <iostream>

#include "LeptonUtils.h"
#include "leptonsensor.h"


LeptonCameraConfig::LeptonCameraConfig(LeptonSensor_t lp) {
  packet_size = 164;
  packet_size_uint16 = packet_size / 2;
  packets_per_segment = 60;
  packets_per_read = 1;  // must be a divisor of packets_per_segment

  segment_size = packet_size * packets_per_segment;
  segment_size_uint16 = packet_size_uint16 * packets_per_segment;

  switch (lp) {
    case LeptonSensor_t::LEPTON2:
      segments_per_frame = 1;
      segment_number_packet_index = 0;
      reset_wait_time = 1000;
      spi_speed = 16000000;  // 16MHz
      width = 80;
      height = 60;
      break;

    case LeptonSensor_t::LEPTON3:
      segments_per_frame = 4;
      segment_number_packet_index = 20;
      reset_wait_time = 1000;
      spi_speed = 32000000;  // 32MHz
      width = 160;
      height = 120;
      break;

    default:
      std::cerr << "Error: Unknown Lepton version.";
      throw std::runtime_error("Unknown Lepton version.");
  }
}

// Open communication with Lepton
bool LeptonSensor::open_connection() {
  // Open I2C
  try {
    leptonI2C_connect();
    usleep(LeptonLoadTime);
    // Read sensor type and prepare config params
    m_config = LeptonCameraConfig(getInfo_sensor());
  } catch (...) {
    std::cerr << "Unable to open connection (I2C) with the sensor."
              << std::endl;
    return false;
  }

  // Open SPI port
  try {
    leptonSPI_OpenPort(m_spi_port, m_config.spi_speed);
  } catch (...) {
    std::cerr << "Unable to open connection (SPI) with the sensor."
              << std::endl;
    return false;
  }

  // Prepare frame buffer
  // Note: each image line comes with 4 bytes header
  m_frame_buffer.resize((m_config.width + 4) * m_config.height);

  return true;
}

// Close communication with Lepton
bool LeptonSensor::close_connection() {
  try {
    // Close SPI port
    leptonSPI_ClosePort(m_spi_port);

    // Close I2C port
    leptonI2C_disconnect();
  } catch (...) {
    std::cerr << "Unable to close connection (I2C/SPI) with the sensor."
              << std::endl;
    return false;
  }

  return true;
}

// Reset communication with Lepton
bool LeptonSensor::reset_connection() {
  bool result_close = close_connection();
  usleep(LeptonResetTime);
  bool result_open = open_connection();
  return result_close && result_open;
}

// Reset SPI communication with Lepton
bool LeptonSensor::reset_SPI_connection() {
  // Close SPI port
  try {
    leptonSPI_ClosePort(m_spi_port);
  } catch (...) {
    std::cerr << "Unable to close connection (SPI) with the sensor."
              << std::endl;
    return false;
  }

  usleep(LeptonResetTime);

  // Open spi port
  try {
    leptonSPI_OpenPort(m_spi_port, m_config.spi_speed);
  } catch (...) {
    std::cerr << "Unable to open connection (SPI) with the sensor."
              << std::endl;
    return false;
  }
  return true;
}

// Reboot sensor and reset connection
bool LeptonSensor::reboot_sensor() {
  leptonI2C_Reboot();  // This function can return a false value even when
                       // reboot succeed, due to the I2C read after reboot,
                       // avoid using the returned value for now
  bool result_close = close_connection();
  usleep(LeptonRebootTime);
  bool result_open = open_connection();
  return result_close && result_open;
}

// Lepton convert frame from sensor to IR imageU8
void LeptonSensor::LeptonUnpackFrame8(uint8_t *frame) {
  // Prepare buffer pointers
  const uint32_t frame_size{m_config.segments_per_frame *
                            m_config.segment_size_uint16};
  auto frame_u8 = reinterpret_cast<uint8_t *>(m_frame_buffer.data());

  // Compute min and max
  uint16_t minValue = 65535;
  uint16_t maxValue = 0;
  for (uint32_t i = 0; i < frame_size; i++) {
    // Skip the first 2 uint16_t's of every packet, they're 4 bytes header
    if (i % m_config.packet_size_uint16 < 2) {
      continue;
    }

    // Flip the MSB and LSB at the last second
    uint8_t temp = frame_u8[i * 2];
    frame_u8[i * 2] = frame_u8[i * 2 + 1];
    frame_u8[i * 2 + 1] = temp;

    // Check min/max value
    uint16_t value = m_frame_buffer[i];
    if (value > maxValue) {
      maxValue = value;
    }
    if (value < minValue) {
      minValue = value;
    }
  }

  // Scale frame range
  auto diff = static_cast<float>(maxValue - minValue);
  float scale = 255.f / diff;
  int idx = 0;
  for (uint32_t i = 0; i < frame_size; i++) {
    // Skip the first 2 uint16_t's of every packet, they're 4 bytes header
    if (i % m_config.packet_size_uint16 < 2) {
      continue;
    }

    frame[idx++] = static_cast<uint8_t>((m_frame_buffer[i] - minValue) *
                                        static_cast<int>(scale));
  }
}

// Lepton convert frame from sensor to IR imageU16
void LeptonSensor::LeptonUnpackFrame16(uint16_t *frame) {
  auto src = reinterpret_cast<uint8_t *>(m_frame_buffer.data());
  auto dst = reinterpret_cast<uint8_t *>(frame);
  int idx = 0;
  const uint32_t frame_size{m_config.segments_per_frame *
                            m_config.segment_size};
  for (uint32_t i = 0; i < frame_size; i += 2) {
    // Skip the first 2 uint16_t's of every packet, they're 4 header bytes
    if (i % m_config.packet_size < 4) {
      continue;
    }

    // Flip the MSB and LSB
    dst[idx++] = src[i + 1];
    dst[idx++] = src[i];
  }
}

// Lepton read segment from sensor
int LeptonSensor::LeptonReadSegment(const int max_resets,
                                    uint8_t *data_buffer) {
  int resets{-1};
  const int step{m_config.packets_per_read};
  const int bytes_per_SPI_read{step * m_config.packet_size};

  for (int j = 0; j < m_config.packets_per_segment; j += step) {
    // Try to reach sync first
    if (j == 0) {
      uint8_t packetNumber{255};
      uint8_t discard_packet{0x0F};
      while (packetNumber != 0 ||
             discard_packet == 0x0F) {  // while packet id is not 0, or the
                                        // packet is a discard packet
        ++resets;
        if (resets == max_resets) {
          return resets;
        }

        usleep(m_config.reset_wait_time);
        read(spi_fd, data_buffer, m_config.packet_size);
        packetNumber = data_buffer[1];
        discard_packet = data_buffer[0] & 0x0F;
      }
      read(spi_fd, data_buffer + m_config.packet_size,
           (step - 1) * m_config.packet_size);
      continue;
    }

    // Check reset counter
    if (resets == max_resets) {
      return resets;
    }

    // Read a packet
    read(spi_fd, data_buffer + j * m_config.packet_size, bytes_per_SPI_read);

    // Checks discard packet
    auto discard_packet = data_buffer[j * m_config.packet_size] & 0x0F;
    if (discard_packet == 0x0F) {
      usleep(m_config.reset_wait_time);
      ++resets;
      j = -step;
      continue;
    }

    // Checks first packet id
    uint8_t packetNumber = data_buffer[j * m_config.packet_size + 1];
    if (packetNumber != j) {
      usleep(m_config.reset_wait_time);
      ++resets;
      j = -step;  // reset just the segment
      continue;
    }
  }

  return resets;
}

// Read frame from lepton sensor
int LeptonSensor::LeptonReadFrame() {
  // Compute packet index for the packet containing the segment ID
  const int segmentId_packet_idx{m_config.segment_number_packet_index *
                                 m_config.packet_size};

  // Read data packets from lepton over SPI
  auto buffer = reinterpret_cast<uint8_t *>(m_frame_buffer.data());
  uint16_t resets{0};
  uint16_t resetsToReboot{0};
  int16_t num_segments{static_cast<int16_t>(m_config.segments_per_frame)};
  for (int16_t segment = 0; segment < num_segments; ++segment) {
    // Check if reset SPI connection is required
    if (resets > MaxResetsPerFrame) {
      resets = 0;
      segment = -1;
      LeptonResync(resetsToReboot);
      continue;
    }

    // Read segment
    uint8_t *data_buffer = buffer + segment * m_config.segment_size;
    int segment_num_resets =
        LeptonReadSegment(MaxResetsPerSegment, data_buffer);
    if (segment_num_resets == MaxResetsPerSegment) {
      segment = -1;
      LeptonResync(resetsToReboot);
      continue;
    }

    // If Lepton module with more than 1 segment
    if (m_config.segments_per_frame > 1) {
      // Checks segment number
      int16_t segmentNumber =
          static_cast<int16_t>((data_buffer[segmentId_packet_idx] >> 4) - 1);
      if (segmentNumber != segment) {
        ++resets;
        segment = -1;  // reset all segments
        continue;
      }
    }
  }

  return resets;
}

void LeptonSensor::LeptonResync(uint16_t &resetsToReboot) {
  // Re-sync by reboot
  if (resetsToReboot > MaxResetsBeforeReboot) {
    resetsToReboot = 0;
    if (!reboot_sensor()) {
      throw std::runtime_error("Unable to reboot sensor.");
    }
  }
  // Re-sync be connection reset
  else {
    ++resetsToReboot;
    if (!reset_SPI_connection()) {
      throw std::runtime_error("Unable to reset SPI connection.");
    }
  }
}

// Lepton get IR frame from sensor
bool LeptonSensor::getFrame(void *frame, LeptonFrame_t type) {
  // Force reboot if user signaled one
  if (m_force_reboot == true) {
    if (!reboot_sensor()) {
      throw std::runtime_error("Unable to reboot sensor.");
    }
    m_force_reboot = false;
  }

  // Read data packets from Lepton over SPI
  // TODO: add a time out, and throw an error when a new frame can't be read
  LeptonReadFrame();

  // Convert Lepton frame to IR frame
  switch (type) {
    case LeptonFrame_t::FRAME_U8:
      LeptonUnpackFrame8(static_cast<uint8_t *>(frame));
      break;
    case LeptonFrame_t::FRAME_U16:
      LeptonUnpackFrame16(static_cast<uint16_t *>(frame));
      break;
    default:
      std::cerr << "Unknown frame type." << std::endl;
      return false;
  }
  m_count++;

  return true;
}

// Lepton I2C command
bool LeptonSensor::send_command(LeptonI2CCmd cmd, void *buffer) {
  // Run FFC
  bool result{false};
  switch (cmd) {
  case LeptonI2CCmd::RESET: {
      result = reset_connection();
      break;
    }
    case LeptonI2CCmd::REBOOT: {
      result = true;
      m_force_reboot = true;
      break;
    }
    case LeptonI2CCmd::FFC: {
      result = leptonI2C_FFC();
      break;
    }
    case LeptonI2CCmd::SENSOR_TEMP_K: {
      auto frame_int = static_cast<unsigned int *>(buffer);
      frame_int[0] = leptonI2C_InternalTemp();
      result = frame_int[0] != 0;
      break;
    }
    case LeptonI2CCmd::SHUTTER_OPEN: {
      result = leptonI2C_ShutterOpen();
      break;
    }
    case LeptonI2CCmd::SHUTTER_CLOSE: {
      result = leptonI2C_ShutterClose();
      break;
    }
    default: {
      std::cerr << "Unknown I2C command." << std::endl;
      result = false;
      break;
    }
  }

  return result;
}

// Get lepton version
LeptonSensor_t LeptonSensor::getInfo_sensor() {
  auto it = MapLepton.find(leptonI2C_SensorNumber());
  if (it != MapLepton.end()) {
    return it->second;
  }

  return LeptonSensor_t::UNKNOWN;
}


