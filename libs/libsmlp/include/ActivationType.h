/**
 * @file ActivationType.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Activation type
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
/**
 * The ActivationType enum defines a set of constants that represent the
 * different types of activation functions that are available. For example, the
 * kSigmoid constant might represent the sigmoid activation function, while the
 * kReLU constant might represent the rectified linear unit(ReLU) activation
 * function.
 */
enum class ActivationType { kSigmoid, kTanh, kReLU, kLeakyReLU };
