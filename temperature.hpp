#ifndef TEMPERATURE_HPP
#define TEMPERATURE_HPP

constexpr double ZEROKELVIN{-273.15};

// formulas to convert temperatures from / to kelvin
constexpr double kelvin_to_celsius(double kelvin_temp) {
  return (kelvin_temp + ZEROKELVIN);
}
constexpr double celsius_to_kelvin(double celsius_temp) {
  return (celsius_temp - ZEROKELVIN);
};

constexpr double kelvin_to_fahrenheit(double kelvin_temp) {
  return ((kelvin_temp * 1.8) - 459.67);
}

constexpr double fahrenheit_to_kelvin(double fahrenheit) {
  return ((fahrenheit + 459.67) / 1.8);
}

// The Lepton camera uses the internal temperature as a parameter. The output
// frame for the internal temperature is 8192. If the range of the temperature
// is not too big, the whole output range will follow an almost linear
// relationship  with this point ( Internal Temperature, 8192).
constexpr double internal_temperature{8192};

class Temperature {
 public:
  Temperature();
  ~Temperature();
  void read_from_sensor();
  double find_max_min_temperature();
  void frame_to_temperature(const uint16_t &frame_max, const uint16_t &frame_min);


 private:
  double m_raw_temp;
  double m_temperature;
};

#endif  // TEMPERATURE_HPP
