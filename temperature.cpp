#include <cstdint>

#include "Lepton_I2C.h"
#include "log/logger.h"
#include "temperature.hpp"

Temperature::Temperature() : m_raw_temp{0.0}, m_temperature{0.0} {
 #if LOGGER
   LOG(INFO, "ctor Temperature")
   LOG(DEBUG, "initialize raw temperature %lf, m_temperature %lf", m_raw_temp,
       m_temperature)
 #endif
}

Temperature::~Temperature() {}

void Temperature::read_from_sensor() {
  m_raw_temp = static_cast<double>(leptonI2C_InternalTemp());
 #if LOGGER
   LOG(TRACE, "acquire raw temperature from sensor %lf", m_raw_temp)
 #endif
}

// Get the maximum and minimum temperature[edit]
// With that information we are able to get an approximated temperature from
// any output. In order to do that we took an experimental point ( Temperature
// and output) using the hot plate available in the Michigan Tech Lab. After
// this, we did a linear equation using two points : ( Internal Temp , 8192)
// and ( Experimental Temperature , Experimental Output ) . You can see the
// equation below: Temperature( Kelvin)  = (8192 - (Internal Temperature  * (
// (Experimental Frame- 8192 )/ ( Experimental Temperature - Internal
// Temperature ))) If you analyse the original code in the file "Lepton
// Thread.h", you can see that you are able to take the maximum and minimum
// output frame. Therefore , if you use the Maximum and Minimum output on the
// equation above, you can find the Maximum and Minimum temperature.
double Temperature::find_max_min_temperature() {
  double sperimental = 25.0;
  double numerator = internal_temperature * (sperimental - internal_temperature);
  double denom = sperimental - internal_temperature;
  double temperature = internal_temperature - (numerator / denom);
  return temperature;
}

// Frame and Temperature Relationship[edit]
// we can realize that:

// The maximum frame is related with the maximum temperature.
// The minimum frame is related with the minimum temperature.
// Using these two points above and the linearity we can find the relationship
// between frame and temperature.

// FrameMax --> TempMax
// FrameMin --> TempMin
// Frame = ((FrameMax - FrameMin) / ( TempMax - TempMin)) * (Temperature -
// TempMax) + FrameMax
void Temperature::frame_to_temperature(const uint16_t &frame_max,
                                       const uint16_t &frame_min) {

#if LOGGER
LOG(DEBUG, "slope %lf", this->find_max_min_temperature())
LOG(DEBUG, "min temperature %lf", kelvin_to_celsius(static_cast<double>(frame_min)))
LOG(DEBUG, "max temperature %lf", kelvin_to_celsius(static_cast<double>(frame_max)))
#endif
}
