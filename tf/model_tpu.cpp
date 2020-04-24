#include "model_tpu.hpp"

#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QRectF>
#include <QString>
#include <QThread>

#include "colormanager.hpp"
#include "image_utils.hpp"
#include "instrumentor.h"
#include "logger.h"
#include "tensorflow/lite/examples/label_image/bitmap_helpers_impl.h"
#include "tensorflow/lite/examples/label_image/get_top_n_impl.h"
#include "tensorflow/lite/kernels/internal/tensor.h"
#include "tensorflow/lite/kernels/internal/tensor_utils.h"

namespace tfclassif = tflite::label_image;

constexpr float kThreshold{0.001F};

ModelTensorFlowLite::ModelTensorFlowLite()
    : wanted_height_(0),
      wanted_width_(0),
      wanted_channels_(3),
      //      has_detection_mask_(false),
      num_threads_(QThread::idealThreadCount()) {
  LOG(LevelAlert::I, "ctor model tensorflow lite")
  LOG(LevelAlert::D, "ideal thread count: %d", num_threads_)
}

void ModelTensorFlowLite::InitializeModelTFLite(const std::string &path) {
  // open model and assign error reporter
  try {
    model =
        tflite::FlatBufferModel::BuildFromFile(path.c_str(), &error_reporter);
    if (model == nullptr) {
      LOG(LevelAlert::F, "can't load TensorFLow lite model from: ", path)
    }

    // link model and resolver
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);
    if (!interpreter) {
      LOG(LevelAlert::F, "failed builder interpreter")
      std::abort();
    }

    // Apply accelaration (Neural Network Android)
    //    interpreter->UseNNAPI(accelaration);

    if (interpreter->AllocateTensors() != kTfLiteOk) {
      LOG(LevelAlert::F, "failed to allocate tensor")
      std::abort();
    }

    if (interpreter->outputs().size() > 1) {
      kind_network_ = TypeDetection::ObjectDetection;
    } else {
      kind_network_ = TypeDetection::ImageClassifier;
    }

    // Get input dimension from the input tensor metadata
    // Assuming one input only
    int input = interpreter->inputs()[0];
    TfLiteIntArray *dims = interpreter->tensor(input)->dims;

    // Save outputs
    outputs.clear();
    for (unsigned int i = 0; i < interpreter->outputs().size(); ++i) {
      outputs.push_back(interpreter->tensor(interpreter->outputs()[i]));
    }
    // set desire tensor image
    wanted_height_ = dims->data[1];
    wanted_width_ = dims->data[2];
    wanted_channels_ = dims->data[3];
    // set number threads
    if (num_threads_ > 1) {
      interpreter->SetNumThreads(num_threads_);
    }
    LOG(LevelAlert::I, "Tensorflow initialization: OK")

#if LOGGER_CNN
    LOG(LevelAlert::I, "verbose mode enable")
    auto i_size = interpreter->inputs().size();
    auto o_size = interpreter->outputs().size();
    auto t_size = interpreter->tensors_size();

    qDebug() << "tensors size: " << t_size;
    qDebug() << "nodes size: " << interpreter->nodes_size();
    qDebug() << "inputs: " << i_size;
    qDebug() << "outputs: " << o_size;

    for (auto i = 0; i < i_size; i++)
      qDebug() << "input" << i << "name:" << interpreter->GetInputName(i)
               << ", type:"
               << interpreter->tensor(interpreter->inputs()[i])->type;

    for (auto i = 0; i < o_size; i++)
      qDebug() << "output" << i << "name:" << interpreter->GetOutputName(i)
               << ", type:"
               << interpreter->tensor(interpreter->outputs()[i])->type;

    for (auto i = 0; i < t_size; i++) {
      if (interpreter->tensor(i)->name)
        qDebug() << i << ":" << interpreter->tensor(i)->name << ","
                 << interpreter->tensor(i)->bytes << ","
                 << interpreter->tensor(i)->type << ","
                 << interpreter->tensor(i)->params.scale << ","
                 << interpreter->tensor(i)->params.zero_point;
    }

    qDebug() << "wanted height:" << wanted_height_;
    qDebug() << "wanted width:" << wanted_width_;
    qDebug() << "wanted channels:" << wanted_channels_;
#endif

  } catch (...) {
    LOG(LevelAlert::F, "can't load TensorFLow lite model from: ", path)
    std::abort();
  }
}

void ModelTensorFlowLite::LoadModelFromFile(const std::string &path) {
  InitializeModelTFLite(path);
}

void ModelTensorFlowLite::LoadModelFromFile(const QString &path) {
  InitializeModelTFLite(path.toStdString());
}

void ModelTensorFlowLite::setLabel(
    const std::unordered_map<int, std::string> &l) {
  m_labels = l;
}

void ModelTensorFlowLite::RunInference(const QImage &image) {
  LOG(LevelAlert::D, "RunInference")
  PROFILE_FUNCTION();
  // detect kind input
  int input = interpreter->inputs()[0];
  TfLiteType input_type = interpreter->tensor(input)->type;
  LOG(LevelAlert::D, "detect input type")
  switch (input_type) {
    case kTfLiteFloat32:
      LOG(LevelAlert::D, "case kTfLiteFloat32")
      resize_image<float>(interpreter->typed_tensor<float>(input), image.bits(),
                          image.height(), image.width(), channels_,
                          wanted_height_, wanted_width_, wanted_channels_,
                          input_type);
      break;
    case kTfLiteInt8:
      LOG(LevelAlert::D, "case kTfLiteInt8")
      resize_image<int8_t>(interpreter->typed_tensor<int8_t>(input),
                           image.bits(), image.height(), image.width(),
                           channels_, wanted_height_, wanted_width_,
                           wanted_channels_, input_type);
      break;
    case kTfLiteUInt8:
      LOG(LevelAlert::D, "case kTfLiteUInt8")
      resize_image<uint8_t>(interpreter->typed_tensor<uint8_t>(input),
                            image.bits(), image.height(), image.width(),
                            channels_, wanted_height_, wanted_width_,
                            wanted_channels_, input_type);
      break;
    default:
      LOG(LevelAlert::F, "cannot handle input type yet",
          interpreter->tensor(input)->type)
      std::exit(-1);
  }
  for (int i = 0; i < 2; i++) {
    interpreter->Invoke();
    if (interpreter->Invoke() != kTfLiteOk) {
      LOG(LevelAlert::F, "Failed to invoke tflite!")
    }
  }

  switch (kind_network_) {
    case TypeDetection::ImageClassifier:
      ClassifierOutput();
      break;

    case TypeDetection::ObjectDetection: {
      ObjectOutput(image);
      auto partial_result = object_detect_->getResult();
      if (partial_result.size() > 0) {
        for (auto &r : partial_result) {
          int cls = r.index_class;
          auto score = r.score;
          auto label = QString("%1: %2 %")
                           .arg(QString::fromStdString(getLabel(cls)))
                           .arg(QString::number(score * 100, 'g', 4));
          r.name = label;
          emit objAvailable(r);
        }
      }
    } break;

    default:
      return;
  }
}

void ModelTensorFlowLite::ClassifierOutput() {
  int output = interpreter->outputs()[0];
  TfLiteIntArray *output_dims = interpreter->tensor(output)->dims;
  // assume output dims to be something like (1, 1, ... ,size)
  auto output_size = output_dims->data[output_dims->size - 1];
  size_t number_of_results = 5;
  auto input_type = interpreter->tensor(output)->type;
  switch (interpreter->tensor(output)->type) {
    case kTfLiteFloat32:
      tfclassif::get_top_n<float>(interpreter->typed_output_tensor<float>(0),
                                  output_size, number_of_results, kThreshold,
                                  &top_results, input_type);
      break;
    case kTfLiteInt8:
      tfclassif::get_top_n<int8_t>(interpreter->typed_output_tensor<int8_t>(0),
                                   output_size, number_of_results, kThreshold,
                                   &top_results, input_type);
      break;
    case kTfLiteUInt8:
      tfclassif::get_top_n<uint8_t>(
          interpreter->typed_output_tensor<uint8_t>(0), output_size,
          number_of_results, kThreshold, &top_results, input_type);
      break;
    default:
      LOG(LevelAlert::F, "cannot handle output type yet",
          interpreter->tensor(output)->type)
      std::exit(-1);
  }
}

void ModelTensorFlowLite::ObjectOutput(const QImage img) {
  object_detect_ = std::make_unique<ObjectDetection>();
  object_detect_->searchObject(outputs, kThreshold, img);
}

std::string ModelTensorFlowLite::getLabel(int i) {
  auto it = m_labels.find(i);
  LOG(LevelAlert::D, "search for class: ", i, "found: ", it->second)
  return it->second;
}

std::vector<BoxDetection> ModelTensorFlowLite::getResults() const {
  return object_detect_->getResult();
}

std::vector<std::pair<float, int> >
ModelTensorFlowLite::getResultClassification() const {
  return top_results;
}

////////////////////////////////////////////////////////////////////////////////
/// Slot
////////////////////////////////////////////////////////////////////////////////

void ModelTensorFlowLite::imageAvailable(QPixmap image) {
  if (!image.isNull()) {
    LOG(LevelAlert::D, "image not null: ", !image.isNull() ? "true" : "false")
    QImage input = image.toImage().convertToFormat(QImage::Format_RGB888);
    RunInference(input);
  }
}

void ModelTensorFlowLite::imageAvailable(QImage image) {
  if (!image.isNull()) {
    LOG(LevelAlert::D, "image not null: ", !image.isNull() ? "true" : "false")
    QImage input = image.convertToFormat(QImage::Format_RGB888);
    RunInference(input);
  }
}
