#ifndef LABEL_UTILS_HPP
#define LABEL_UTILS_HPP

#include <QString>
#include <string>
#include <tuple>

#include "logger.h"

using callback_split = std::tuple<int, std::string> (*)(const QString &);

/**
 * @brief LabelSplitter class collects static methods which allow to interpret
 * some files containing the labels used for the training of the neural model.
 *
 */
class LabelSplitter {
 public:
  /**
   * @brief reads a text file containing labels in COCO format and returns a
   * tuple containing index and label.
   *
   * \n
   * 0  person\n
   * 1  bicycle\n
   * 2  car\n
   * 3  motorcycle\n
   * 4  airplane\n
   * 5  bus\n
   * 6  train\n
   * 7  truck\n
   * 8  boat\n
   * 9  traffic light\n
   * 10  fire hydrant\n
   * ...\n
   *
   * @param str path to the file.
   * @return std::tuple<int, std::string> containing index and label.
   */
  static std::tuple<int, std::string> CocoLabel(const QString &str);

  /**
   * @brief reads a text file containing labels in Imagenet format and returns a
   * tuple containing index and label.
   *
   * \n
   * 0  background\n
   * 1  tench, Tinca tinca\n
   * 2  goldfish, Carassius auratus\n
   * 3  great white shark, white shark, man-eater, man-eating shark,
   * Carcharodon carcharias\n
   * 4  tiger shark, Galeocerdo cuvieri\n
   * 5  hammerhead, hammerhead shark\n
   * 6  electric ray, crampfish, numbfish, torpedo\n
   * 7  stingray\n
   * 8  cock\n
   * 9  hen\n
   * 10  ostrich, Struthio camelus\n
   * ...\n
   *
   * @param str path to the file.
   * @return std::tuple<int, std::string> containing index and label.
   */
  static std::tuple<int, std::string> ImagenetLabel(const QString &str);

  /**
   * @brief reads a text file containing labels in format and returns a tuple
   * containing index and label.
   *
   * \n
   * 0:background\n
   * 1:tench, Tinca tinca\n
   * 2:goldfish, Carassius auratus\n
   * 3:great white shark, white shark, man-eater, man-eating shark, Carcharodon
   * carcharias\n
   * 4:tiger shark, Galeocerdo cuvieri\n
   * 5:hammerhead, hammerhead shark\n
   * 6:electric ray, crampfish, numbfish, torpedo\n
   * 7:stingray\n
   * 8:cock\n
   * 9:hen\n
   * 10:ostrich, Struthio camelus\n
   * ...\n
   *
   * @param str path to the file.
   * @return std::tuple<int, std::string> containing index and label.
   */
  static std::tuple<int, std::string> GenericLabel(const QString &str);
};

#endif  // LABEL_UTILS_HPP
