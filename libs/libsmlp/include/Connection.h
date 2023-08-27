/**
 * @file Connection.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Connection between two neurons
 * @date 2023-08-26
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
class Connection
{
public:
    Connection(int source_unit, int destination_unit, float weight)
        : source_unit_(source_unit), destination_unit_(destination_unit), weight_(weight) {}

    // Source unit in the layer n
    int GetSourceUnit() const { return source_unit_; }

    // Destination unit in the layer n+1
    int GetDestinationUnit() const { return destination_unit_; }

    float GetWeight() const { return weight_; }
    void SetWeight(float weight) { weight_ = weight; }

private:
    int source_unit_;
    int destination_unit_;
    float weight_;
};
