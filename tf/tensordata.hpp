#ifndef TENSOR_DATA_HPP
#define TENSOR_DATA_HPP

#include "tensordata_impl.hpp"

/**
 * @brief TensorData function which allows adapt the TensorFlow tensor for
 * retrive result's inference.
 *
 * @tparam T Type of tensor admit float, int8_t, uint8_t.
 * @param tensor result of the inference depending on the type of model, it may
 * be necessary to specify the index to access the specific field.
 *
 * @param batch_index number of elements processed, generally set to 0
 * @return T* tensor.
 */
template <typename T>
T *TensorData(TfLiteTensor *tensor, int batch_index);

#endif  // TENSOR_DATA_HPP
