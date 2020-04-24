#include "labels.hpp"

#include <QFile>
#include <QRegularExpression>
#include <QString>
#include <QTextStream>
#include <iostream>
#include <utility>

#include "label_utils.hpp"
#include "logger.h"

LabelDetection::LabelDetection(QString path) : m_filename(std::move(path)) {}

LabelDetection::LabelDetection(const std::string &path)
    : m_filename(QString::fromStdString(path)) {}

void LabelDetection::IdentifyTypeFile() {
  if (m_filename.endsWith(".txt") && m_filename.contains("imagenet")) {
    m_process_line = LabelSplitter::ImagenetLabel;
  }

  else if (m_filename.endsWith(".txt") && m_filename.contains("coco")) {
    m_process_line = LabelSplitter::CocoLabel;
  }

  else if (m_filename.endsWith(".txt") && !m_filename.contains("imagenet") &&
           !m_filename.contains("coco")) {
    m_process_line = LabelSplitter::GenericLabel;
  }

  else {
    LOG(LevelAlert::F, "file unsupported")
    std::cerr << "file unsupported: " << m_filename.toStdString() << "\n";
    std::abort();
  }
}

std::unordered_map<int, std::string> LabelDetection::getLabels() {
  return m_labels;
}

void LabelDetection::read() {
  IdentifyTypeFile();
  QFile file(m_filename);
  if (!file.open(QIODevice::ReadOnly)) {
    LOG(LevelAlert::E, "file not exist ", m_filename.toStdString())
    return;
  }
  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();
    auto [label_id, label_name] = m_process_line(line);
    m_labels[label_id] = label_name;
  }
}
