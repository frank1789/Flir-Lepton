#ifndef TENSOR_DATA_IMPL_HPP
#define TENSOR_DATA_IMPL_HPP

#include <cstdint>

#include "logger.h"
#include "tensorflow/lite/builtin_op_data.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"

// --------------------------------------------------------------------------------------
// Code from:
// https://github.com/YijinLiu/tf-cpu/blob/master/benchmark/obj_detect_lite.cc
// --------------------------------------------------------------------------------------
template <typename T>
T *TensorData(TfLiteTensor *tensor, int batch_index);

template <>
float *TensorData<float>(TfLiteTensor *tensor, int batch_index) {
  int nelems = 1;
  for (int i = 1; i < tensor->dims->size; i++) nelems *= tensor->dims->data[i];
  switch (tensor->type) {
    case kTfLiteFloat32:
      return tensor->data.f + nelems * batch_index;
    default:
      break;
  }
  return nullptr;
}

template <>
int8_t *TensorData<int8_t>(TfLiteTensor *tensor, int batch_index) {
  int nelems = 1;
  for (int i = 1; i < tensor->dims->size; i++) nelems *= tensor->dims->data[i];
  switch (tensor->type) {
    case kTfLiteInt8:
      return tensor->data.int8 + nelems * batch_index;
    default:
      break;
  }
  return nullptr;
}

template <>
uint8_t *TensorData<uint8_t>(TfLiteTensor *tensor, int batch_index) {
  int nelems = 1;
  for (int i = 1; i < tensor->dims->size; i++) nelems *= tensor->dims->data[i];
  switch (tensor->type) {
    case kTfLiteInt8:
      return tensor->data.uint8 + nelems * batch_index;
    default:
      break;
  }
  return nullptr;
}

#endif  // TENSOR_DATA_IMPL_HPP
