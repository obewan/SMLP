/**
 * @file InputOutputPair.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Input/Ouput pair
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include <vector>

class InputOutputPair
{
public:
    InputOutputPair(const std::vector<float> &input, const std::vector<float> &output)
        : input_(input), output_(output) {}

    const std::vector<float> &GetInput() const
    {
        return input_;
    }

    const std::vector<float> &GetOutput() const
    {
        return output_;
    }

private:
    std::vector<float> input_;
    std::vector<float> output_;
};
