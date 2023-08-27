/**
 * \mainpage SMLP
 * A Simple Multi-Layer Perceptron to study artificial networks.
 * \author  Damien Balima (https://dams-labs.net)
 * \date    August 2023
 * \copyright
 *
 * [![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International
License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]:
https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg

- *Attribution*: you must give appropriate credit, provide a link to the
license, and indicate if changes were made. You may do so in any reasonable
manner, but not in any way that suggests the licensor endorses you or your use.
- *NonCommercial*: you may not use the material for commercial purposes.
- *ShareAlike*: if you remix, transform, or build upon the material, you must
distribute your contributions under the same license as the original.
- *No additional restrictions*: you may not apply legal terms or technological
measures that legally restrict others from doing anything the license permits.
 */

#include "AdamOptimizer.h"
#include "Network.h"
#include "Training.h"
#include <string>

int main() {
  // Create instances of Network, Optimizer, and TrainingData
  size_t input_size = 1;
  size_t hidden_size = 1;
  size_t output_size = 1;
  float learning_rate = 1;
  float beta1 = 1;
  float beta2 = 1;
  const std::string data_file_path = "../test/training_data.txt";

  Optimizer *optimizer = new AdamOptimizer(learning_rate, beta1, beta2);
  Network network(input_size, hidden_size, output_size, optimizer,
                  learning_rate);

  Training training(&network, data_file_path, optimizer);

  // Train the network using online training
  training.Train(3);

  return 0;
}