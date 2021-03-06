#ifndef LEPTON_I2C_H
#define LEPTON_I2C_H

#include "LeptonSDKEmb32PUB/LEPTON_SDK.h"
#include "LeptonSDKEmb32PUB/LEPTON_SYS.h"
#include "LeptonSDKEmb32PUB/LEPTON_Types.h"

// I2C constant
constexpr LEP_UINT16 I2CPortID{1};
constexpr LEP_UINT16 I2CPortBaudRate{400};
constexpr LEP_CAMERA_PORT_E I2CPortType{LEP_CCI_TWI};

// Kelvin constant
constexpr double KELVINCELSIUS{273.15};

// port connection
extern bool _connected;
extern LEP_CAMERA_PORT_DESC_T _port;

//============================================================================
// Lepton I2C Commands
//============================================================================

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
 * @brief Run FFC
 *
 */
void lepton_perform_ffc();

double leptonI2C_InternalTemp();

#endif  // LEPTON_I2C_H
