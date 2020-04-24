#ifndef MODEL_TPU_HPP
#define MODEL_TPU_HPP

#include <QObject>
#include <memory>
#include <string>

#include "object_detection.hpp"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

QT_BEGIN_NAMESPACE
class QImage;
class QString;
class QPixmap;
class QString;
class QRectF;
QT_END_NAMESPACE

enum class TypeDetection : int { ImageClassifier, ObjectDetection };

class ModelTensorFlowLite : public QObject {
  Q_OBJECT
 public:
  explicit ModelTensorFlowLite();
  void InitializeModelTFLite(const std::string &path);

  // setter
  void LoadModelFromFile(const std::string &path);
  void LoadModelFromFile(const QString &path);
  void setLabel(const std::unordered_map<int, std::string> &l);

  // core
  void RunInference(const QImage &image);
  void ClassifierOutput();
  void ObjectOutput(const QImage img);

  // getter
  std::string getLabel(int i);
  std::vector<BoxDetection> getResults() const;
  std::vector<std::pair<float, int>> getResultClassification() const;

 signals:
  void objAvailable(BoxDetection);

 public slots:
  void imageAvailable(QPixmap image);
  void imageAvailable(QImage image);

 private:
  // input image properties
  const int channels_{3};

  // size tensor image desired
  int wanted_height_;
  int wanted_width_;
  int wanted_channels_;
  TypeDetection kind_network_{TypeDetection::ObjectDetection};
  int num_threads_;
  std::vector<std::pair<float, int>> top_results;
  std::unordered_map<int, std::string> m_labels;
  std::unique_ptr<ObjectDetection> object_detect_{nullptr};

  // model
  std::unique_ptr<tflite::FlatBufferModel> model{nullptr};
  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::StderrReporter error_reporter;
  std::unique_ptr<tflite::Interpreter> interpreter{nullptr};
  std::vector<TfLiteTensor *> outputs;
};

#endif  // MODEL_TPU_HPP
