#include "logger.h"

// define constant color hex
const char RED[] = "\x1b[0;31m";
const char GREEN[] = "\x1b[0;32m";
const char YELLOW[] = "\x1b[0;33m";
const char BLUE[] = "\x1b[0;34m";
const char MAGENTA[] = "\x1b[0;35m";
const char CYAN[] = "\x1b[0;36m";
const char GREY[] = "\x1b[0;36m";
const char LIGHT_GREY[] = "\x1b[0;37m";
const char LIGHT_RED[] = "\x1b[0;91m";
const char LIGHT_GREEN[] = "\x1b[0;92m";
const char LIGHT_YELLOW[] = "\x1b[0;93m";
const char LIGHT_BLUE[] = "\x1b[0;94m";
const char LIGHT_MAGENTA[] = "\x1b[0;95m";
const char LIGHT_CYAN[] = "\x1b[0;96m";
const char WHITE[] = "\x1b[0;97m";
const char RESET[] = "\x1b[0m";

// initialize level's name
const char *levels_name[] = {"TRACE",   "DEBUG", "INFO",
                             "WARNING", "ERROR", "FATAL"};

// initialize level's color
const char *levels_color[] = {YELLOW, CYAN, GREEN, LIGHT_RED, RED, MAGENTA};

void logger(level_t level, const char *file, int line, const char *fmt, ...) {
  va_list args;
  fprintf(stderr, "[%s %s %s %s:%d] ", levels_color[level], levels_name[level],
          RESET, file, line);
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fflush(stderr);
}

void TimeMeter::start() { m_start = std::chrono::high_resolution_clock::now(); }

void TimeMeter::stop() { m_stop = std::chrono::high_resolution_clock::now(); }

void TimeMeter::getPartialElapsed() {
  m_partial_elapsed = m_stop - m_start;
  std::cout << "Partial time elapsed: " << m_partial_elapsed.count()
            << " [us] ";
  std::cout << "(" << m_partial_elapsed.count() * 0.001 << "[ms])"
            << "\n";
}

void TimeMeter::getTotalElapsed() {
  m_partial_elapsed = m_stop - m_start;
  m_total_elapsed += m_partial_elapsed;
  std::cout << "Total time elapsed: " << m_total_elapsed.count() << " [us] ";
  std::cout << "(" << m_total_elapsed.count() * 0.001 << "[ms])"
            << "\n";
}

TimeMeter::~TimeMeter() {
  LOG(FATAL, "Call Dtor time meter")
  m_total_elapsed = m_stop - m_start;
  std::cout << "Total time elapsed: " << m_total_elapsed.count() << " [us] ";
  std::cout << "(" << m_total_elapsed.count() * 0.001 << " [ms])"
            << "\n";
}

StopWatch::StopWatch() : m_start(std::chrono::high_resolution_clock::now()) {}

StopWatch::~StopWatch() {
  m_stop = std::chrono::high_resolution_clock::now();
  auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_start)
                   .time_since_epoch()
                   .count();
  auto stop = std::chrono::time_point_cast<std::chrono::microseconds>(m_stop)
                  .time_since_epoch()
                  .count();
  auto total_elapsed = stop - start;
  LOG(TRACE, "Time elapsed: %6.3lf [us]; %6.3lf [ms]",
      static_cast<double>(total_elapsed),
      static_cast<double>(total_elapsed * 0.001))
}
