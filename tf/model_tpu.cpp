#include "model_tpu.hpp"

#include <tensorflow/lite/examples/label_image/get_top_n_impl.h>
#include <tensorflow/lite/kernels/internal/tensor.h>
#include <tensorflow/lite/kernels/internal/tensor_utils.h>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>

#include "../log/instrumentor.h"
#include "../log/logger.h"
#include "../src/streamerthread.hpp"
#include "colormanager.hpp"
#include "model_support_function.hpp"

#define LOG_CNN 1

constexpr float min_threshold{0.6f};

ModelTensorFlowLite::ModelTensorFlowLite() : QObject() {
  threshold = min_threshold;
  img_height = 512;
  img_width = 512;
  wanted_height = 0;
  wanted_width = 0;
  wanted_channels = 3;
  has_detection_mask = false;
  kind_network = type_detection::none;
  numThreads = 1;
  LOG(INFO, "ctor model tensorflow lite")
}

ModelTensorFlowLite::ModelTensorFlowLite(const QString &path)
    : QObject(),
      threshold(min_threshold),
      img_height(512),
      img_width(512),
      wanted_height(0),
      wanted_width(0),
      wanted_channels(3),
      has_detection_mask(false),
      kind_network(type_detection::none),
      numThreads(1) {
  LOG(INFO, "ctor model tensorflow lite")
  LOG(DEBUG, "load model from resources %s", path.toStdString().c_str())
  init_model_TFLite(path.toStdString());
}

void ModelTensorFlowLite::setLabel(
    const std::unordered_map<int, std::string> &l) {
  m_labels = l;
}

void ModelTensorFlowLite::imageAvailable(QPixmap image) {
  if (!image.isNull()) {
    LOG(DEBUG, "image not null: %s", !image.isNull() ? "true" : "false")
    QImage input = image.toImage().convertToFormat(QImage::Format_RGB888);
    run(input);
  }
}

void ModelTensorFlowLite::run(QImage image) {
  LOG(DEBUG, "run inference tflite")
  StopWatch tm;
  PROFILE_FUNCTION();
  // check input
  if (image.isNull()) {
    LOG(WARN, "check image is not valid: %s\nthen return",
        image.isNull() ? "true" : "false")
    return;
  }
  setInput(image);
  // perform inference
  if (interpreter->Invoke() != kTfLiteOk) {
    LOG(ERROR, "failde to invoke intepreter")
    return;
  }
  // check if classifier or object detection
  LOG(DEBUG, "detect network: %d", kind_network)
  switch (kind_network) {
    case type_detection::image_classifier: {
      std::vector<std::pair<float, int>> top_results;
      if (!get_classifier_output(&top_results)) {
        LOG(DEBUG, "empty result")
        return;
      }
      break;
    }
    case type_detection::object_detection: {
      LOG(DEBUG, "retrive object detection result")
      if (!get_object_outputs()) {
        LOG(DEBUG, "empty result")
        return;
      }
      break;
    }
  }
}

void ModelTensorFlowLite::init_model_TFLite(const std::string &path) {
  // open model and assign error reporter
  try {
    model =
        tflite::FlatBufferModel::BuildFromFile(path.c_str(), &error_reporter);
    if (model == nullptr) {
      LOG(FATAL, "can't load TensorFLow lite model from: %", path.c_str())
    }
    // link model and resolver
    tflite::InterpreterBuilder builder(*model.get(), resolver);
    // Check interpreter
    if (builder(&interpreter) != kTfLiteOk) {
      LOG(ERROR, "interpreter is not ok")
    }

    // Apply accelaration (Neural Network Android)
    //    interpreter->UseNNAPI(accelaration);

    if (interpreter->AllocateTensors() != kTfLiteOk) {
      LOG(ERROR, "failed to allocate tensor")
    }

    // Set kind of network
    kind_network = interpreter->outputs().size() > 1
                       ? type_detection::object_detection
                       : type_detection::image_classifier;

#if LOG_CNN
    LOG(INFO, "verbose mode enable")
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
#endif

    // Get input dimension from the input tensor metadata
    // Assuming one input only
    int input = interpreter->inputs()[0];
    TfLiteIntArray *dims = interpreter->tensor(input)->dims;

    // Save outputs
    outputs.clear();
    for (unsigned int i = 0; i < interpreter->outputs().size(); ++i)
      outputs.push_back(interpreter->tensor(interpreter->outputs()[i]));

    wanted_height = dims->data[1];
    wanted_width = dims->data[2];
    wanted_channels = dims->data[3];

#if LOG_CNN
    qDebug() << "Wanted height:" << wanted_height;
    qDebug() << "Wanted width:" << wanted_width;
    qDebug() << "Wanted channels:" << wanted_channels;
#endif

    if (numThreads > 1) interpreter->SetNumThreads(numThreads);
    LOG(INFO, "Tensorflow initialization: OK")
  } catch (...) {
    LOG(FATAL, "can't load TensorFLow lite model from: %", path.c_str())
    std::abort();
  }
}

void ModelTensorFlowLite::setInput(QImage image) {
  // get inputs
  std::vector<int> inputs = interpreter->inputs();
  // set inputs
  for (unsigned int i = 0; i < interpreter->inputs().size(); ++i) {
    auto input = inputs[i];
    // convert input
    switch (interpreter->tensor(input)->type) {
      case kTfLiteFloat32:
        formatImageTFLite<float>(interpreter->typed_tensor<float>(input),
                                 image.bits(), image.height(), image.width(),
                                 m_num_channels, wanted_height, wanted_width,
                                 wanted_channels, true);
        break;
      case kTfLiteUInt8:
        formatImageTFLite<uint8_t>(interpreter->typed_tensor<uint8_t>(input),
                                   image.bits(), img_height, img_width,
                                   m_num_channels, wanted_height, wanted_width,
                                   wanted_channels, false);
        break;
      default:
        LOG(WARN, "Cannot handle input type %s yet",
            interpreter->tensor(input)->type)
        return;
    }
  }
}

bool ModelTensorFlowLite::get_classifier_output(
    std::vector<std::pair<float, int>> *top_results) {
  bool status{false};
  const int output_size = 1000;
  const size_t num_results = 5;
  // Assume one output
  if (interpreter->outputs().size() > 0) {
    int output = interpreter->outputs()[0];
    switch (interpreter->tensor(output)->type) {
      case kTfLiteFloat32: {
        tflite::label_image::get_top_n<float>(
            interpreter->typed_output_tensor<float>(0), output_size,
            num_results, threshold, top_results, true);
        status = true;
        break;
      }
      case kTfLiteUInt8: {
        tflite::label_image::get_top_n<uint8_t>(
            interpreter->typed_output_tensor<uint8_t>(0), output_size,
            num_results, threshold, top_results, false);
        status = true;
        break;
      }
      default: {
        LOG(ERROR, "Cannot handle output type %s yet",
            interpreter->tensor(output)->type)
        status = false;
      }
    }
  }
  return status;
}

bool ModelTensorFlowLite::get_object_outputs() {
  bool status{false};
  if (outputs.size() >= 4) {
    const int num_detections =
        static_cast<int>(*TensorData<float>(outputs[3], 0));
    const float *detection_classes = TensorData<float>(outputs[1], 0);
    const float *detection_scores = TensorData<float>(outputs[2], 0);
    const float *detection_boxes = TensorData<float>(outputs[0], 0);
    const float *detection_masks = !has_detection_mask || outputs.size() < 5
                                       ? nullptr
                                       : TensorData<float>(outputs[4], 0);
    ColorManager cm;

    for (int i = 0; i < num_detections; i++) {
      // Get class
      const int cls = static_cast<int>(detection_classes[i]);
      // Ignore first one
      if (cls == 0) continue;
      // Get score
      auto score = detection_scores[i];
      // Check minimum score
      if (score < threshold) {
        LOG(WARN, "low score: %3.3lf, class %s", static_cast<double>(score),
            getLabel(cls).toStdString().c_str())
        break;
      }
      // Get class label
      const QString label = getLabel(cls);
      // Get coordinates
      const float top = detection_boxes[4 * i] * img_height;
      const float left = detection_boxes[4 * i + 1] * img_width;
      const float bottom = detection_boxes[4 * i + 2] * img_height;
      const float right = detection_boxes[4 * i + 3] * img_width;
      // Save coordinates
      QRectF box(left, top, right - left, bottom - top);
      // Get masks
      // WARNING: Under development
      // https://github.com/matterport/Mask_RCNN/issues/222
      if (detection_masks != nullptr) {
        const int dim1 = outputs[4]->dims->data[2];
        const int dim2 = outputs[4]->dims->data[3];
        QImage mask(dim1, dim2, QImage::Format_ARGB32_Premultiplied);
        // Set binary mask [dim1,dim2]
        for (int j = 0; j < mask.height(); j++) {
          for (int k = 0; k < mask.width(); k++) {
            auto index = i * dim1 * dim2 + j * dim2 + k;
            auto check = detection_masks[index] >= MASK_THRESHOLD;
            auto fill =
                (check == true) ? cm.getColor(label) : QColor(Qt::transparent);
            mask.setPixel(k, j, fill.rgba());
          }
        }

        // Billinear interpolation
        // https://chu24688.tian.yam.com/posts/44797337
        // QImage maskScaled =
        //
        ColorManager::billinearInterpolation(mask, box.height(), box.width());

        // Scale mask to box size
        QImage maskScaled = mask.scaled(
            static_cast<int>(box.width()), static_cast<int>(box.height()),
            Qt::IgnoreAspectRatio, Qt::FastTransformation);

        // Border detection
        //         QTransform trans(-1,0,1,-2,0,2,-1,0,1);
        //         maskScaled =
        //         ColorManager::applyTransformation(maskScaled,trans);

        // Append to masks
        //        result->masks.append(maskScaled);
      }
      // Append data
      LOG(DEBUG, "label: %s, score: %3.3lf", label.toStdString().c_str(),
          static_cast<double>(score))
      //      result->caption.append(label);
      //      result->confidences.append(static_cast<double>(score));
      //      result->box.append(box);
      status = true;
    }
  }
  return status;
}

QString ModelTensorFlowLite::getLabel(int i) {
  std::unordered_map<int, std::string>::iterator it = m_labels.find(i);
  LOG(DEBUG, "search for class %d, found %s", i, it->second.c_str())
  return QString::fromStdString(it->second);
}
