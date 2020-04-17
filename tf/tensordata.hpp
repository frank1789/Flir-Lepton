#ifndef TENSOR_DATA_HPP
#define TENSOR_DATA_HPP

#include "tensordata_impl.hpp"

template <typename T>
T *TensorData(TfLiteTensor *tensor, int batch_index);

#endif  // TENSOR_DATA_HPP
