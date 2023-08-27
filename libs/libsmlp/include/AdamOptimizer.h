/**
 * @file AdamOptimizer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Adam optimizer
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Network.h"
#include "Layer.h"
#include "Optimizer.h"
class AdamOptimizer : public Optimizer
{
public:
    // Constructor that takes the learning rate and the decay rates for the moving averages as arguments
    AdamOptimizer(float learning_rate, float beta1, float beta2);

    // Method that updates the weights and biases of the network given the gradients of the loss with respect to the weights and biases
    void UpdateWeightsAndBiases(Layer *layer, float learning_rate) override;

private:
    // Member variables that store the learning rate and the decay rates for the moving averages
    float learning_rate_;
    float beta1_;
    float beta2_;

    // Member variable that stores the time step
    size_t t_;

    // Member variable that specifies the number of iterations after which t_ will be reset to zero
    size_t reset_iterations_;

    // Member variable that stores the small constant to prevent division by zero
    float epsilon_;

    // Member variable that stores the moving averages of the gradients of the weights and biases
    std::vector<float> moving_averages_;

    // Member variable that stores the squared moving averages of the gradients of the weights and biases
    std::vector<float> squared_moving_averages_;
};