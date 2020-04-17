#ifndef LABEL_UTILS_HPP
#define LABEL_UTILS_HPP

#include <QString>
#include <string>
#include <tuple>

#include "logger.h"

using callback_split = std::tuple<int, std::string> (*)(const QString &);

class LabelSplitter {
 public:
  static std::tuple<int, std::string> CocoLabel(const QString &str);
  static std::tuple<int, std::string> ImagenetLabel(const QString &str);
  static std::tuple<int, std::string> GenericLabel(const QString &str);
};

#endif  // LABEL_UTILS_HPP
