#ifndef IMAGE_UTILS_HPP
#define IMAGE_UTILS_HPP

#include "image_utils_impl.hpp"

template <typename T, typename U>
void resize_image(T* out, U* in, int image_height, int image_width,
                  int image_channels, int wanted_height, int wanted_width,
                  int wanted_channels, TfLiteType input_type);

template <typename U>
void resize_image(float* out, U* in, int image_height, int image_width,
                  int image_channels, int wanted_height, int wanted_width,
                  int wanted_channels, TfLiteType input_type);

template <typename U>
void resize_image(int8_t* out, U* in, int image_height, int image_width,
                  int image_channels, int wanted_height, int wanted_width,
                  int wanted_channels, TfLiteType input_type);

template <typename U>
void resize_image(uint8_t* out, U* in, int image_height, int image_width,
                  int image_channels, int wanted_height, int wanted_width,
                  int wanted_channels, TfLiteType input_type);

#endif  // IMAGE_UTILS_HPP