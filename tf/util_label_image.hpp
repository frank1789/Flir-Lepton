#ifndef UTIL_LABEL_IMAGE_HPP
#define UTIL_LABEL_IMAGE_HPP

#include <fstream>
#include <functional>
#include <string>
#include <tuple>
#include <unordered_map>

#include <QObject>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

using callback_split = std::tuple<int, std::string> (*)(const QString &);

class LabelSplitter {
 public:
  static std::tuple<int, std::string> coco_label_split(const QString &str);
  static std::tuple<int, std::string> imagenet_label_split(const QString &str);
  static std::tuple<int, std::string> tensorflow_label_map(const QString &str);
};

std::unordered_map<int, std::string> read_label_file(
    const std::string &file_path, callback_split split);

class LabelDetection {
 public:
  explicit LabelDetection(const QString &path);

  std::unordered_map<int, std::string> getLabels();

  void read();

 private:
  QString m_filename;
  std::unordered_map<int, std::string> m_labels;
  callback_split m_process_line;
};

#endif  // UTIL_LABEL_IMAGE_HPP
