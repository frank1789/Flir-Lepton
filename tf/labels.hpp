#ifndef LABELS_HPP
#define LABELS_HPP

#include <QObject>
#include <string>
#include <unordered_map>

#include "label_utils.hpp"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class LabelDetection {
 public:
  LabelDetection(QString path);
  LabelDetection(const std::string &path);
  void read();

  // getter method
  std::unordered_map<int, std::string> getLabels();

 private:
  // methods
  void IdentifyTypeFile();

  // attributes
  QString m_filename;
  std::unordered_map<int, std::string> m_labels;
  callback_split m_process_line{nullptr};
};

#endif  // LABELS_HPP
