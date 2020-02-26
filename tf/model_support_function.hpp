///
///
///
///
///
///
///
///

#ifndef MODEL_SUPPORT_FUNCTION_HPP
#define MODEL_SUPPORT_FUNCTION_HPP

#include <QDebug>

#include "tensorflow/lite/error_reporter.h"
#include "tensorflow/lite/graph_info.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/internal/tensor.h"
#include "tensorflow/lite/kernels/internal/tensor_utils.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

///
///-----------------------------------------------------------------------------------------------------------------------
///
/// https://github.com/tensorflow/tensorflow/blob/master/tensorflow/contrib/lite/examples/label_image/bitmap_helpers_impl.h
///
///-----------------------------------------------------------------------------------------------------------------------
template <class T>
void formatImageTFLite(T* out, const uint8_t* in, int image_height,
                       int image_width, int image_channels, int wanted_height,
                       int wanted_width, int wanted_channels,
                       bool input_floating) {
  constexpr float input_mean = 127.5f;
  constexpr float input_std = 127.5f;

  int number_of_pixels = image_height * image_width * image_channels;
  std::unique_ptr<tflite::Interpreter> interpreter(new tflite::Interpreter);

  int base_index = 0;

  // two inputs: input and new_sizes
  interpreter->AddTensors(2, &base_index);

  // one output
  interpreter->AddTensors(1, &base_index);

  // set input and output tensors
  interpreter->SetInputs({0, 1});
  interpreter->SetOutputs({2});

  // set parameters of tensors
  TfLiteQuantizationParams quant;
  interpreter->SetTensorParametersReadWrite(
      0, kTfLiteFloat32, "input",
      {1, image_height, image_width, image_channels}, quant);
  interpreter->SetTensorParametersReadWrite(1, kTfLiteInt32, "new_size", {2},
                                            quant);
  interpreter->SetTensorParametersReadWrite(
      2, kTfLiteFloat32, "output",
      {1, wanted_height, wanted_width, wanted_channels}, quant);

  tflite::ops::builtin::BuiltinOpResolver resolver;
  const TfLiteRegistration* resize_op =
      resolver.FindOp(tflite::BuiltinOperator_RESIZE_BILINEAR, 1);
  auto* params = reinterpret_cast<TfLiteResizeBilinearParams*>(
      malloc(sizeof(TfLiteResizeBilinearParams)));
  params->align_corners = false;
  interpreter->AddNodeWithParameters({0, 1}, {2}, nullptr, 0, params, resize_op,
                                     nullptr);
  interpreter->AllocateTensors();

  // fill input image
  // in[] are integers, cannot do memcpy() directly
  auto input = interpreter->typed_tensor<float>(0);
  for (int i = 0; i < number_of_pixels; i++) input[i] = in[i];

  // fill new_sizes
  interpreter->typed_tensor<int>(1)[0] = wanted_height;
  interpreter->typed_tensor<int>(1)[1] = wanted_width;

  interpreter->Invoke();

  auto output = interpreter->typed_tensor<float>(2);
  auto output_number_of_pixels =
      wanted_height * wanted_height * wanted_channels;

  for (int i = 0; i < output_number_of_pixels; i++) {
    if (input_floating)
      out[i] = (output[i] - input_mean) / input_std;
    else
      out[i] = (uint8_t)output[i];
  }
}

// --------------------------------------------------------------------------------------
// Code from:
// https://github.com/YijinLiu/tf-cpu/blob/master/benchmark/obj_detect_lite.cc
// --------------------------------------------------------------------------------------
template <typename T>
T* TensorData(TfLiteTensor* tensor, int batch_index);

template <>
float* TensorData(TfLiteTensor* tensor, int batch_index) {
  int nelems = 1;
  for (int i = 1; i < tensor->dims->size; i++) nelems *= tensor->dims->data[i];
  switch (tensor->type) {
    case kTfLiteFloat32:
      return tensor->data.f + nelems * batch_index;
    default:
      qDebug() << "Should not reach here!";
  }
  return nullptr;
}

template <>
uint8_t* TensorData(TfLiteTensor* tensor, int batch_index) {
  int nelems = 1;
  for (int i = 1; i < tensor->dims->size; i++) nelems *= tensor->dims->data[i];
  switch (tensor->type) {
    case kTfLiteUInt8:
      return tensor->data.uint8 + nelems * batch_index;
    default:
      qDebug() << "Should not reach here!";
  }
  return nullptr;
}

#endif // MODEL_SUPPORT_FUNCTION_HPP
