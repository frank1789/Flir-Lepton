#ifndef OBJECT_DETECTION_HPP
#define OBJECT_DETECTION_HPP

#include <QObject>
#include <memory>
#include <vector>

#include "tensorflow/lite/kernels/internal/tensor.h"
#include "tensorflow/lite/kernels/internal/tensor_utils.h"

QT_BEGIN_NAMESPACE
class QImage;
class QRectF;
QT_END_NAMESPACE

struct BoxDetection{
  int index_class;
  float score;
  qreal left;
  qreal top;
  qreal width;
  qreal heigh;
  QString name;
};

Q_DECLARE_METATYPE(BoxDetection);

class ObjectDetection {
 public:
  explicit ObjectDetection() = default;
  ~ObjectDetection() = default;
  void SearchObject(const std::vector<TfLiteTensor *> &outputs, float threshold,
                    const QImage &img);

  std::vector<BoxDetection> getResult() const;

 private:
  int num_detections_;
  std::unique_ptr<float> detection_classes_{nullptr};
  std::unique_ptr<float> detection_scores_{nullptr};
  std::unique_ptr<float> detection_boxes_{nullptr};
  std::vector<BoxDetection> class_box_;
};

#endif  // OBJECT_DETECTION_HPP
