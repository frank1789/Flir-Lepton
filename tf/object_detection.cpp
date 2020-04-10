#include "object_detection.hpp"

#include <QImage>
#include <QRectF>

#include "tensordata.hpp"

void ObjectDetection::SearchObject(const std::vector<TfLiteTensor *> &outputs,
                                   float threshold, const QImage &img) {
  if (outputs.size() > 3 && outputs.size() < 5) {
    detection_boxes_ =
        std::make_unique<float>(*TensorData<float>(outputs[0], 0));
    detection_classes_ =
        std::make_unique<float>(*TensorData<float>(outputs[1], 0));
    detection_scores_ =
        std::make_unique<float>(*TensorData<float>(outputs[2], 0));
    num_detections_ = static_cast<int>(*TensorData<float>(outputs[3], 0));

    for (int i = 0; i < num_detections_; i++) {
      // get class
      int cls = static_cast<int>(detection_classes_.get()[i]);
      if (cls == 0) continue;  // in general class 0 is background
      auto score = static_cast<float>(detection_scores_.get()[i]);
      if (score < threshold) {
        LOG(LevelAlert::W, "low score: ", score, ", class ", cls)
        break;
      }

      // get corners coordinates
      const auto top =
          static_cast<qreal>(detection_boxes_.get()[4 * i] * img.height());
      const auto left =
          static_cast<qreal>(detection_boxes_.get()[4 * i + 1] * img.width());
      const auto bottom =
          static_cast<qreal>(detection_boxes_.get()[4 * i + 2] * img.height());
      const auto right =
          static_cast<qreal>(detection_boxes_.get()[4 * i + 3] * img.width());
      QRectF box(left, top, right - left, bottom - top);
      LOG(LevelAlert::D, "find score: ", score, ", class: ", cls)
      class_box_.emplace_back(std::make_tuple(cls, score, box));
    }
  }
}

std::vector<std::tuple<int, float, QRectF>> ObjectDetection::getResult() const {
  return class_box_;
}
