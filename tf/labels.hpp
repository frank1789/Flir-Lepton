#ifndef LABELS_HPP
#define LABELS_HPP

#include <QObject>
#include <string>
#include <unordered_map>

#include "label_utils.hpp"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

/**
 * @brief LabelDetection class allows you to import the files containing the
 * labels, necessary for the classification and object detection operation,
 * choosing how to interpret the file based on the name or extension.
 *
 */
class LabelDetection {
 public:
 /**
  * @brief Construct a new Label Detection object
  * 
  * @param path string containing the path to the file.
  */
  LabelDetection(QString path);

  /**
   * @brief Construct a new Label Detection object
   * 
   * @param path string containing the path to the file.
   */
  LabelDetection(const std::string &path);

  /**
   * @brief Read file lines by lines.
   * 
   */
  void read();

  /**
   * @brief Get the Labels object.
   * 
   * @return std::unordered_map<int, std::string> label map.
   */
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
