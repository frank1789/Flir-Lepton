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

/**
 * @brief BoxDetection is a structure that contains information extracted from
 * the output tensor.
 *
 */
struct BoxDetection {
  int index_class; /**< class index */
  float score;     /**< confidence */
  qreal left;   /**< value of the coordinate of an edge of the bounding box */
  qreal top;    /**< value of the coordinate of an edge of the bounding box */
  qreal width;  /**< value of the coordinate of an edge of the bounding box */
  qreal heigh;  /**< value of the coordinate of an edge of the bounding box */
  QString name; /**< object's name associated with the label's index */
};

Q_DECLARE_METATYPE(BoxDetection);

/**
 * @brief Object Detection class performs the inference on the input tensor by
 * returning a vector containing all boxes.
 *
 */
class ObjectDetection {
 public:
  /**
   * @brief Construct a new Object Detection object.
   *
   */
  explicit ObjectDetection() = default;

  /**
   * @brief Destroy the Object Detection object
   *
   */
  ~ObjectDetection() = default;

  /**
   * @brief SearchObject given an input tensor it searches all the elements
   * present in the tensor.
   *
   * The function looks for the number of objects detected in the tensor, the
   * position of the bounding box and the confidence score, note that
   * the input image is necessary to scale bounding box.
   * If the result is valid, it is inserted into the result vector.
   *
   * @param outputs tensor result of inference.
   * @param threshold minimum threshold for which the score is valid, currently
   * uses 0.01 hard-coded.
   * @param img analyzed image
   */
  void searchObject(const std::vector<TfLiteTensor *> &outputs, float threshold,
                    const QImage &img);

  /**
   * @brief Get the Result object.
   *
   * @return std::vector<BoxDetection> results.
   */
  std::vector<BoxDetection> getResult() const;

 private:
  int num_detections_;
  std::unique_ptr<float> detection_classes_{nullptr};
  std::unique_ptr<float> detection_scores_{nullptr};
  std::unique_ptr<float> detection_boxes_{nullptr};
  std::vector<BoxDetection> class_box_;
};

#endif  // OBJECT_DETECTION_HPP
