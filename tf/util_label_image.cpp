#include "util_label_image.hpp"

#include <QFile>
#include <QRegularExpression>
#include <QString>
#include <QTextStream>

#include "../log/instrumentor.h"
#include "../log/logger.h"

std::tuple<int, std::string> LabelSplitter::coco_label_split(
    const QString &str) {
  QString id{""};
  QString label{""};
  QString accessor{""};
  QRegularExpression re;
  re.setPattern(
      "(?<id>[0-9]\\d*)\\s*(?<label>[A-z]\\w*)?\\s+(?<accessor>[A-z]\\w+)");
  QRegularExpressionMatch match = re.match(str);
  if (match.hasMatch()) {
    id = match.captured("id");
    label = match.captured("label");
    accessor = match.captured("accessor");
    if (!accessor.isEmpty() && accessor != "") {
      label = label + " " + accessor;
    }
  }
  return std::make_tuple(id.toUInt(), label.toStdString());
}

std::tuple<int, std::string> LabelSplitter::imagenet_label_split(
    const QString &str) {
  QString id;
  QString label;
  QString accessor;
  QString accessor2;
  QRegularExpression re;
  re.setPattern(
      "(?<id>[0-9]\\d*)\\s*(?<label>[A-z]\\w*)?\\s+(?<accessor>[A-z]\\w*)?\\s+("
      "?<accessor2>[A-z]\\w*)");
  QRegularExpressionMatch match = re.match(str);
  if (match.hasMatch()) {
    id = match.captured("id");
    label = match.captured("label");
    accessor = match.captured("accessor");
    accessor2 = match.captured("accessor2");

    if (!accessor.isEmpty()) {
      label += " " + accessor;
    }
    if (!accessor2.isEmpty()) {
      label += " " + accessor2;
    }
  }
  return std::make_tuple(id.toUInt(), label.toStdString());
}

std::tuple<int, std::string> LabelSplitter::tensorflow_label_map(
    const QString &str) {
  QString id;
  QString label;
  QString accessor;
  QRegularExpression re;
  re.setPattern(
      "(?<id>[0-9]\\d*)\\s*(?<label>[A-z]\\w*)?\\s+(?<accessor>[A-z]\\w+)");
  QRegularExpressionMatch match = re.match(str);
  if (match.hasMatch()) {
    id = match.captured("id");
    label = match.captured("label");
    accessor = match.captured("accessor");
    if (!accessor.isEmpty()) {
      label += " " + accessor;
    }
  }
  return std::make_tuple(id.toUInt(), label.toStdString());
}

std::unordered_map<int, std::string> read_label_file(
    const std::string &file_path, callback_split split) {
  std::unordered_map<int, std::string> labels;
  std::ifstream input_label(file_path);
  if (!input_label.is_open()) {
    LOG(ERROR, "failed to open ", file_path.data())
    std::cerr << "Cannot open file: " << file_path << std::endl;
    std::abort();
  } else {
    std::string line;
    while (getline(input_label, line)) {
      auto [label_id, label_name] = split(QString::fromStdString(line));
      labels[label_id] = label_name;
    }
  }
  return labels;
}

LabelDetection::LabelDetection(const QString &path) : m_filename(path) {
  if (path.endsWith(".txt")) {
    m_process_line = LabelSplitter::coco_label_split;
  } else if (path.endsWith(".pbtxt")) {
    m_process_line = LabelSplitter::tensorflow_label_map;
  } else {
    LOG(ERROR, "unsupported label map %s", path.toStdString().c_str())
  }
}

std::unordered_map<int, std::string> LabelDetection::getLabels() {
  return m_labels;
}

void LabelDetection::read() {
  QFile file(m_filename);
  if (!file.open(QIODevice::ReadOnly)) {
    LOG(ERROR, "file not exist %s", m_filename.toStdString().c_str())
    return;
  }

  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();
    auto [label_id, label_name] = m_process_line(line);
    LOG(DEBUG, "found label: %4d %s", label_id, label_name.c_str())
    m_labels[label_id] = label_name;
  }
}
