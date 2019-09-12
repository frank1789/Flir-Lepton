#ifndef LEPTONUTILS_H
#define LEPTONUTILS_H

#include <LeptonSDKEmb32PUB/LEPTON_Types.h>
#include <cstdint>

//============================================================================
// Lepton SPI Communication
//============================================================================

extern int spi_fd;

/**
 * @brief Open SPI communication
 * @param spi_device  SPI device id
 * @param spi_speed   Desired SPI speed
 * @throw Runtime error if port can't be opened or configured
 */
void leptonSPI_OpenPort(int spi_device, uint32_t spi_speed);

/**
 * @brief Close SPI communication
 * @param spi_device  SPI device id
 * @throw Runtime error if port can't be closed
 */
void leptonSPI_ClosePort(int spi_device);

//============================================================================
// Lepton I2C Commands
//============================================================================

// I2C constant
constexpr LEP_UINT16 I2CPortID{1};
constexpr LEP_UINT16 I2CPortBaudRate{400};
constexpr LEP_CAMERA_PORT_E I2CPortType{LEP_CCI_TWI};

// Kelvin constant
constexpr double KELVINCELSIUS{273.15};


extern bool _connected;
extern LEP_CAMERA_PORT_DESC_T _port;

/**
 * @brief Open I2C communication with Lepton sensor
 * @throw Runtime error if communication can't be established
 */
void leptonI2C_connect();

/**
 * @brief Close I2C communication with Lepton sensor
 * @throw Runtime error if communication can't be closed
 */
void leptonI2C_disconnect();

/**
 * @brief Open shutter
 * @return Return true if operation succeed, false otherwise
 */
bool leptonI2C_ShutterOpen();

/**
 * @brief Run FFC
 * @return Return true if operation succeed, false otherwise
 */
bool leptonI2C_FFC();

/**
 * @brief Close shutter
 * @return Return true if operation succeed, false otherwise
 */
bool leptonI2C_ShutterClose();

/**
 * @brief Reboot lepton sensor
 * @return Return true if operation succeed, false otherwise
 */
bool leptonI2C_Reboot();

/**
 * @brief Set shutter mode to manual
 * @return Return true if operation succeed, false otherwise
 */
bool leptonI2C_ShutterManual();

/**
 * @brief Get thermal sensor core temperature
 * @return Return sensor temperature in Kelvins
 */
unsigned int leptonI2C_InternalTemp();

/**
 * @brief Get thermal sensor number/version
 * @return Return senors number/version
 */
unsigned int leptonI2C_SensorNumber();

#endif  // LEPTONUTILS_H
