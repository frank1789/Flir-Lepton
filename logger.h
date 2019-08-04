#ifndef LOGGER_H
#define LOGGER_H

#if __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>

#define LOG(LEVEL, ...) logger(LEVEL, __FILE__, __LINE__, __VA_ARGS__)

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

const char *levels_name[] = {"TRACE",   "DEBUG", "INFO",
                             "WARNING", "ERROR", "FATAL"};
const char *levels_color[] = {LIGHT_GREY, CYAN, GREEN, LIGHT_RED, RED, MAGENTA};

typedef enum level { TRACE, DEBUG, INFO, WARN, ERROR, FATAL } level_t;

/*
 */
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

#if __cplusplus
}
#endif

#endif  // LOGGER_H
