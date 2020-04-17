#ifndef LOGGER_H
#define LOGGER_H

#include <array>
#include <iostream>
#include <ostream>
#include <string>
#include <type_traits>

template <typename E>
constexpr auto to_underlying(E e) noexcept {
  return static_cast<std::underlying_type_t<E>>(e);
}

#define LOGGER 0
#if LOGGER
#define LOGGER_GEN 1
#define LOGGER_UI 1
#define LOGGER_CNN 1
#define LOG(LEVEL, ...) logger(LEVEL, __FILE__, __LINE__, __VA_ARGS__);
#else
#define LOG(LEVEL, ...)
#endif

// define constant color hex
extern const std::string RED;
extern const std::string GREEN;
extern const std::string YELLOW;
extern const std::string BLUE;
extern const std::string MAGENTA;
extern const std::string CYAN;
extern const std::string GREY;
extern const std::string LIGHT_GREY;
extern const std::string LIGHT_RED;
extern const std::string LIGHT_GREEN;
extern const std::string LIGHT_YELLOW;
extern const std::string LIGHT_BLUE;
extern const std::string LIGHT_MAGENTA;
extern const std::string LIGHT_CYAN;
extern const std::string WHITE;
extern const std::string RESET;

enum class LevelAlert : int { I, T, D, W, E, F };

inline std::string get_level(LevelAlert lvl) {
  std::string alert;
  switch (lvl) {
    case LevelAlert::I:
      alert = "I ";
      break;
    case LevelAlert::T:
      alert = "T ";
      break;
    case LevelAlert::D:
      alert = "D ";
      break;
    case LevelAlert::W:
      alert = "W ";
      break;
    case LevelAlert::E:
      alert = "E ";
      break;
    case LevelAlert::F:
      alert = "F ";
      break;
  }
  return alert;
}

inline std::ostream &operator<<(std::ostream &os, LevelAlert lvl) {
  auto alert = get_level(lvl);
  os << alert;
  return os;
}

inline const std::string &color_out(LevelAlert lvl) {
  switch (lvl) {
    case LevelAlert::I:
      return GREEN;

    case LevelAlert::T:
      return CYAN;

    case LevelAlert::D:
      return CYAN;

    case LevelAlert::W:
      return LIGHT_RED;

    case LevelAlert::E:
      return RED;

    case LevelAlert::F:
      return MAGENTA;

    default:
      return RESET;
  }
}

template <typename... T>
inline void logger(LevelAlert lvl, const char *file, int line, T &&... args) {
  std::cerr << color_out(lvl) << lvl << RESET << " " << file << " l: " << line
            << "] ";
  int expander[]{0, (void(std::cerr << std::forward<T>(args) << " "), 0)...};
  static_cast<void>(expander);
  std::cerr << std::endl;
}

#endif  // LOGGER_H
