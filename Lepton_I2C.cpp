#include "Lepton_I2C.h"
#include "LeptonSDKEmb32PUB/LEPTON_AGC.h"
#include "log/logger.h"

bool _connected{false};
LEP_CAMERA_PORT_DESC_T _port;

// Open Lepton I2C
void lepton_connect() {
  LEP_RESULT result =
      LEP_OpenPort(I2CPortID, LEP_CCI_TWI, I2CPortBaudRate, &_port);
  if (result == LEP_OK) {
    std::cout << "Open I2C port: " << _port.portID << ", with address "
              << static_cast<int>(_port.deviceAddress) << std::endl;
  } else {
    std::cerr << "Unable to open I2C communication.";
    throw std::runtime_error("I2C connection failed");
  }
  _connected = true;
}

// Close Lepton I2C
void leptonI2C_disconnect() {
  LEP_RESULT result = LEP_ClosePort(&_port);
  if (result == LEP_OK) {
    std::cout << "Close I2C port: " << _port.portID << ", with address "
              << static_cast<int>(_port.deviceAddress) << std::endl;
  } else {
    std::cerr << "Unable to close I2C communication.";
    throw std::runtime_error("I2C close connection failed");
  }
  _connected = false;
}

void lepton_perform_ffc() {
  if (!_connected) {
    lepton_connect();
  }
  LEP_RunSysFFCNormalization(&_port);
}

// Get internal temperature
unsigned int leptonI2C_InternalTemp() {
  if (!_connected) {
    lepton_connect();
  }
  LEP_SYS_FPA_TEMPERATURE_KELVIN_T fpa_temp_kelvin{0};
  LEP_RESULT result = LEP_GetSysFpaTemperatureKelvin(&_port, &fpa_temp_kelvin);
#if LOGGER
  LOG(DEBUG, "FPA raw temperature: %i, code %i", fpa_temp_kelvin, result)
#endif
  return fpa_temp_kelvin;
}

unsigned int leptonI2C_AuxTemp() {
  if (!_connected) {
    lepton_connect();
  }
  LEP_SYS_FPA_TEMPERATURE_KELVIN_T aux_temp_kelvin{0};
  LEP_RESULT result = LEP_GetSysAuxTemperatureKelvin(&_port, &aux_temp_kelvin);
#if LOGGER
  LOG(DEBUG, "AUX raw temperature: %i, code %i", aux_temp_kelvin, result)
#endif
  return aux_temp_kelvin;
}

void lepton_disableAGC() {
  if (!_connected) {
    lepton_connect();
  }
  LEP_RESULT result = LEP_SetAgcEnableState(&_port, LEP_AGC_DISABLE);
#if LOGGER
  LOG(DEBUG, "disable AGC: code %i", result)
#endif
}

void lepton_enableAGC() {
  if (!_connected) {
    lepton_connect();
  }
  LEP_RESULT result = LEP_SetAgcEnableState(&_port, LEP_AGC_ENABLE);
#if LOGGER
  LOG(DEBUG, "enable AGC: code %i", result)
#endif
}

unsigned long long int lepton_get_serial_number() {
  if (!_connected) {
    lepton_connect();
  }
  unsigned long long int serial_number{0};
  LEP_RESULT result = LEP_GetSysFlirSerialNumber(&_port, &serial_number);
#if LOGGER
  LOG(DEBUG, "read serial number lepton camera: %i, code %i", serial_number,
      result)
#endif
  return serial_number;
}

unsigned int lepton_get_uptime() {
  if (!_connected) {
    lepton_connect();
  }
  unsigned int up_time{0};
  LEP_RESULT result = LEP_GetSysCameraUpTime(&_port, &up_time);
#if LOGGER
  LOG(DEBUG, "lepton camera up time: %i, code %i", up_time, result)
#endif
  return (up_time);
}

// presumably more commands could go here if desired
