#include <algorithm>
#include "Network.h"
#include "InputLayer.h"
#include <stdexcept>

using namespace std;

Network::Network(size_t input_size, size_t hidden_size, size_t output_size, Optimizer *optimizer, float learning_rate) : optimizer_(optimizer), learning_rate_(learning_rate)
{
	// Initialize the input layer
	input_layer_ = new InputLayer(input_size, new ActivationFunction(ActivationType::kSigmoid));

	// Initialize the hidden layer
	auto hidden_layer = new HiddenLayer(hidden_size, new ActivationFunction(ActivationType::kSigmoid), input_layer_);
	hidden_layers_.push_back(hidden_layer);

	// Initialize the output layer
	output_layer_ = new OutputLayer(output_size, new ActivationFunction(ActivationType::kSigmoid), hidden_layer);

	// Connect the layers
	input_layer_->ConnectTo(hidden_layer);
	hidden_layer->ConnectTo(output_layer_);
}

size_t Network::NumWeights() const
{
	size_t num_weights = 0;
	for (int i = 0; i < hidden_layers_.size(); ++i)
	{
		num_weights += hidden_layers_[i]->NumUnits() * (i == 0 ? input_layer_->NumUnits() : hidden_layers_[i - 1]->NumUnits() + 1);
	}
	num_weights += output_layer_->NumUnits() * hidden_layers_.back()->NumUnits() + output_layer_->NumUnits();
	return num_weights;
}

size_t Network::NumBiases() const
{
	size_t num_biases = 0;
	for (auto const &hidden_layer : hidden_layers_)
	{
		num_biases += hidden_layer->NumUnits();
	}
	num_biases += output_layer_->NumUnits();
	return num_biases;
}

/**
 * Calculate the layer and weight indices based on the given index
 * and iterates through the hidden layers to find the corresponding weight value
 */
float Network::GetHiddenWeight(int index) const
{
	int layer_index = 0;
	int weight_index = index;
	while (weight_index >= hidden_layers_[layer_index]->Connections().size())
	{
		weight_index -= hidden_layers_[layer_index]->Connections().size();
		layer_index++;
	}

	const Connection &connection = hidden_layers_[layer_index]->Connections()[weight_index];
	return connection.GetWeight();
}

void Network::SetHiddenWeight(int index, float value)
{
	int layer_index = 0;
	int weight_index = index;
	while (weight_index >= hidden_layers_[layer_index]->Connections().size())
	{
		weight_index -= hidden_layers_[layer_index]->Connections().size();
		layer_index++;
	}

	Connection &connection = hidden_layers_[layer_index]->Connections()[weight_index];
	connection.SetWeight(value);
}

float Network::GetBias(int index) const
{
	int layer_index = 0;
	int bias_index = index;
	while (bias_index >= hidden_layers_[layer_index + 1]->NumUnits())
	{
		bias_index -= hidden_layers_[layer_index + 1]->NumUnits();
		layer_index++;
	}
	return hidden_layers_[layer_index + 1]->GetBias(bias_index);
}

void Network::SetBias(int index, float value)
{
	int layer_index = 0;
	int bias_index = index;
	while (bias_index >= hidden_layers_[layer_index + 1]->NumUnits())
	{
		bias_index -= hidden_layers_[layer_index + 1]->NumUnits();
		layer_index++;
	}
	hidden_layers_[layer_index + 1]->SetBias(bias_index, value);
}

float *Network::Forward(std::vector<float> &input_values)
{
	// Set the input values of the input layer
	input_layer_->SetInput(input_values);

	// Compute the output values of each layer in the network
	for (auto hidden_layer : hidden_layers_)
	{
		hidden_layer->ComputeOutput();
	}
	output_layer_->ComputeOutput();

	// Return a pointer to the output values
	return output_layer_->GetUnitValues().data();
}

void Network::AddHiddenLayer(int num_units, ActivationFunction *activation_function, Layer *previous_layer)
{
	auto hidden_layer = new HiddenLayer(num_units, activation_function, previous_layer);
	hidden_layers_.push_back(hidden_layer);
}

void Network::Backward()
{
	// Compute the gradients of the loss with respect to the output layer's unit values
	output_layer_->ComputeGradients();

	// Propagate the gradients backward through the hidden layers
	for (int i = (int)hidden_layers_.size() - 1; i >= 0; i--)
	{
		auto const &hidden_layer = hidden_layers_.at(i);
		hidden_layer->ComputeGradients();
	}

	// Update the weights and biases using the computed gradients
	optimizer_->UpdateWeightsAndBiases(input_layer_, learning_rate_);
	input_layer_->ClearGradients();
	for (auto layer : hidden_layers_)
	{
		optimizer_->UpdateWeightsAndBiases(layer, learning_rate_);
		layer->ClearGradients();
	}
	optimizer_->UpdateWeightsAndBiases(output_layer_, learning_rate_);
	output_layer_->ClearGradients();
}

void Network::ClearGradient()
{
	// Loop over the layers in the network
	for (auto layer : hidden_layers_)
	{
		layer->ClearGradients();
	}
}

std::vector<float> Network::Predict(std::vector<float> &input)
{
	// Set the input layer activations
	input_layer_->SetInput(input);

	// Loop over the layers in the network
	for (auto layer : hidden_layers_)
	{
		// Compute the activations of the neurons in the layer
		layer->ComputeOutput();
	}

	// Return the activations of the output layer
	return output_layer_->GetUnitValues();
}

void Network::UpdateWeightsAndBiases()
{
	// Update the weights and biases using the optimizer for each hidden layer
	for (auto hidden_layer : hidden_layers_)
	{
		optimizer_->UpdateWeightsAndBiases(hidden_layer, learning_rate_);
	}

	// Update the weights and biases for the output layer using the optimizer
	optimizer_->UpdateWeightsAndBiases(output_layer_, learning_rate_);
}
