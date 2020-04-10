#include "list_colour.hpp"

#include "logger.h"

QColor def_color(const QList<colorwheel_t> &colour, int index) {
  const auto &[i, name_colour, code_colour] = colour.at(index % colour.count());
  LOG(LevelAlert::D, "colour selected: ", i, name_colour.toStdString())
  return code_colour.toRgb();
}
