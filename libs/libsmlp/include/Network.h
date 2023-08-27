/**
 * @file Network.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Neural network class that contains layers and main methods
 * @date 2023-08-26
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */

#pragma once
#include "Layer.h"
#include "HiddenLayer.h"
#include "OutputLayer.h"
#include "InputLayer.h"
#include "Optimizer.h"
#include <vector>

/**
 The methods for getting and setting weights and biases are also necessary
 for implementing backpropagation during training.
 The addition of InitializeWeights() will also be useful
 for initializing the weights in the network before training.

 The Forward() method takes a std::vector<float> input and returns a float* output.
 It's important to make sure that the memory for the output is allocated
 before passing it to the ComputeOutput() method of the output layer.

The Backward() and ClearGradient() methods will be used during backpropagation
to compute the gradients of the weights and biases in the network.

Finally, the Predict() method takes a std::vector<float> input and returns a std::vector<float> output.
This method will be useful for using the trained network to make predictions on new input data.
*/

class Network
{
public:
	// Constructor that takes the sizes of the input, hidden, and output layers as arguments
	Network(size_t input_size, size_t hidden_size, size_t output_size, Optimizer *optimizer, float learning_rate);

	// Method that returns the total number of weights in the network
	size_t NumWeights() const;

	// Method that returns the total number of biases in the network
	size_t NumBiases() const;

	// Method that returns a pointer to the specified layer in the network
	InputLayer *GetInputLayer()
	{
		return input_layer_;
	};

	HiddenLayer *GetHiddenLayer(int index)
	{
		return hidden_layers_.at(index);
	};

	OutputLayer *GetOutputLayer()
	{
		return output_layer_;
	};

	void AddHiddenLayer(int num_units, ActivationFunction *activation_function, Layer *previous_layer);

	float *Forward(std::vector<float> &input_values);

	void Backward();

	void ClearGradient();

	float GetHiddenWeight(int index) const;
	void SetHiddenWeight(int index, float value);

	float GetBias(int index) const;
	void SetBias(int index, float value);

	void UpdateWeightsAndBiases();

	std::vector<float> Predict(std::vector<float> &input);

private:
	InputLayer *input_layer_;
	std::vector<HiddenLayer *> hidden_layers_;
	OutputLayer *output_layer_;
	Optimizer *optimizer_;
	float learning_rate_;

	// A vector of connection layers weights
	std::vector<double> connections_;
};
