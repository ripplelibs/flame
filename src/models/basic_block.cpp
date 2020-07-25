//==--- src/models/basic_block.cpp ------------------------- -*- C++ -*- ---==//
//
//                                Flame
//
//                      Copyright (c) 2020 Flame
//
//  This file is distributed under the MIT License. See LICENSE for details.
//
//==------------------------------------------------------------------------==//
//
/// \file  basic_block.cpp
/// \brief Implementation file for a basic block for a resenet.
//
//==------------------------------------------------------------------------==//

#include <flame/models/basic_block.hpp>
#include <flame/util/conv.hpp>

namespace flame::models {

BasicBlockImpl::BasicBlockImpl(
  int64_t     input_channels,
  int64_t     output_channels,
  int64_t     stride,
  DownSampler downsampler)
: _conv_1{conv_3x3(input_channels, output_channels, stride)},
  _batchnorm_1{output_channels},
  _conv_2{conv_3x3(output_channels, output_channels)},
  _batchnorm_2{output_channels},
  _relu{torch::nn::ReLUOptions().inplace(true)},
  _downsampler{downsampler} {
  register_module("conv1", _conv_1);
  register_module("bn1", _batchnorm_1);
  register_module("conv2", _conv_2);
  register_module("bn2", _batchnorm_2);
  register_module("relu", _relu);

  if (_downsampler) {
    register_module("downsample", _downsampler);
  }
}

auto BasicBlockImpl::forward(torch::Tensor x) -> torch::Tensor {
  auto out = _conv_1->forward(x);
  out      = _batchnorm_1->forward(out);
  out      = _relu->forward(out);
  out      = _conv_2->forward(out);
  out      = _batchnorm_2->forward(out);

  auto residual = _downsampler ? _downsampler->forward(x) : x;
  out += residual;
  out = _relu->forward(out);

  return out;
}

} // namespace flame::models