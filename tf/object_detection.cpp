#include "object_detection.hpp"

#include "tensordata.hpp"

void ObjectDetection::SearchObject(const std::vector<TfLiteTensor *> &outputs,
                                   float threshold, const QImage &img,
                                   int max_index_class) {
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
      if (cls == 0 || cls <= max_index_class) continue;
      auto score = static_cast<float>(detection_scores_.get()[i]);
      if (score < threshold || score <= 1.00f) {
        LOG(LevelAlert::W, "low score: ", score, ", class ", cls)
        break;
      }

      // get corners coordinates
      const auto top =
          static_cast<float>(detection_boxes_.get()[4 * i] * img.height());
      const auto left =
          static_cast<float>(detection_boxes_.get()[4 * i + 1] * img.width());
      const auto bottom =
          static_cast<float>(detection_boxes_.get()[4 * i + 2] * img.height());
      const auto right =
          static_cast<float>(detection_boxes_.get()[4 * i + 3] * img.width());

      if (score <= 1.00f && cls < max_index_class) {
        LOG(LevelAlert::D, "append to vector find score: ", score,
            ", class: ", cls)
        class_box_.push_back({cls,score,left, top, right - left, bottom - top, ""});
      } else {
        BoxDetection empty_result;
        class_box_.push_back(empty_result);
      }
    }
  }
}

std::vector<BoxDetection> ObjectDetection::getResult() const {
  return class_box_;
}
