#ifndef MODEL_TPU_HPP
#define MODEL_TPU_HPP

#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>

#include <QObject>
#include <QThread>
#include <memory>
#include <string>

QT_BEGIN_NAMESPACE
class QImage;
class QString;
class QByteArray;
QT_END_NAMESPACE

enum type_detection { none = -1, image_classifier = 1, object_detection = 2 };

struct result_t {
  // Results
  QList<QString> caption;
  QList<double> confidences;
  QList<QRectF> box;
  QList<QImage> masks;
};

class ModelTensorFlowLite : public QObject {
 public:
  explicit ModelTensorFlowLite();
  explicit ModelTensorFlowLite(const QString &path);

  void run(QImage image);

  void setInput(QImage image);

  bool get_classifier_output(std::vector<std::pair<float, int> > *top_results);
  bool get_object_outputs();

  void setLabel(const std::unordered_map<int, std::string> &l);
  QString getLabel(int i);
 public slots:
  void imageAvailable(QPixmap image);

 private:
  //  methods
  void init_model_TFLite(const std::string &path);
  bool getObjectOutputsTFLite(QStringList &captions, QList<double> &confidences,
                              QList<QRectF> &locations, QList<QImage> &masks);

  // constants
  static constexpr float MASK_THRESHOLD{0.3f};

  // parametric threshold
  float threshold;

  // output string
  const QString m_num_detections = "num_detections: ";
  const QString m_detection_classes = "detection_classes: ";
  const QString m_detection_scores = "detection_scores: ";
  const QString m_detection_boxes = "detection_boxes: ";
  const QString m_detection_masks = "detection_masks: ";

  // image properties
  const int m_num_channels{3};
  int img_height, img_width;
  int wanted_height, wanted_width, wanted_channels;

  // detection mask
  bool has_detection_mask;

  //
  int kind_network;

  // thread
  int numThreads;

  result_t m_result;

  std::unordered_map<int, std::string> m_labels;

  // model
  std::unique_ptr<tflite::FlatBufferModel> model;
  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::StderrReporter error_reporter;
  std::unique_ptr<tflite::Interpreter> interpreter;
  std::vector<TfLiteTensor *> outputs;
};

#endif  // MODEL_TPU_HPP
