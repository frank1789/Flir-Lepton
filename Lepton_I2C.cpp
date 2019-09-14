#include "Lepton_I2C.h"
#include "logger.h"

bool _connected{false};
LEP_CAMERA_PORT_DESC_T _port;

// Open Lepton I2C
void lepton_connect() {
    LEP_RESULT result = LEP_OpenPort(I2CPortID, LEP_CCI_TWI, I2CPortBaudRate, &_port);
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
	if(!_connected) {
		lepton_connect();
	}
	LEP_RunSysFFCNormalization(&_port);
}

// Get internal temperature
double leptonI2C_InternalTemp() {
    LEP_SYS_FPA_TEMPERATURE_KELVIN_T fpa_temp_kelvin{0};
    if (_connected) {
        LEP_RESULT result =
            LEP_GetSysFpaTemperatureKelvin(&_port, &fpa_temp_kelvin);
#if LOGGER
        LOG(DEBUG, "FPA raw temperature: %i, code %i", fpa_temp_kelvin, result)
        LOG(DEBUG, "temperature K: %i, code %i", fpa_temp_kelvin/100, result)
#endif
    }
    return static_cast<double>((fpa_temp_kelvin)/100);
}

//presumably more commands could go here if desired
