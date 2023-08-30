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
#include "Testing.h"
#include "Training.h"
#include "include/CLI11.hpp"
#include <cstddef>
#include <iostream>
#include <string>

/**
 * @brief main function
 *
 * @param argc numbers of arguments
 * @param argv table of arguments: 1:input_size 2:hidden_size
 * @code {.bash}
 * smlp -f ../test/mushroom/mushroom_data.csv -i 20 -s 20 -o 1 -e 10
 * -l 40000 -x false
 * @endcode
 *
 * @return int
 */
int main(int argc, char *argv[]) {
  // Parsing arguments
  CLI::App app{"SMLP"};
  std::string data_file = "";
  size_t input_size = 0;
  size_t hidden_size = 0;
  size_t output_size = 0;
  size_t num_epochs = 0;
  size_t to_line = 0;
  bool output_at_end = false;

  app.add_option("-f,--file_input", data_file,
                 "the data file to use for training and testing")
      ->mandatory()
      ->check(CLI::ExistingPath);
  app.add_option("-i,--input_size", input_size, "the numbers of input neurons")
      ->mandatory()
      ->check(CLI::PositiveNumber);
  app.add_option("-s,--hidden_size", hidden_size,
                 "the numbers of hidden neurons")
      ->default_val(10)
      ->check(CLI::PositiveNumber);
  app.add_option("-o,--output_size", output_size,
                 "the numbers of output neurons")
      ->default_val(1)
      ->check(CLI::PositiveNumber);
  app.add_option("-e,--epochs", num_epochs, "the numbers of epochs retraining")
      ->default_val(10)
      ->check(CLI::PositiveNumber);
  app.add_option(
         "-l,--line_to", to_line,
         "the line number until the training will complete and testing will "
         "start, or 0 to use the entire file")
      ->default_val(0)
      ->check(CLI::NonNegativeNumber);
  app.add_option(
         "-x,--output_ends", output_at_end,
         "indicate if the output data is at the end of the record (1) or at "
         "the beginning (0)")
      ->default_val(false)
      ->check(CLI::TypeValidator<bool>());

  CLI11_PARSE(app, argc, argv);

  // Create instances of Network, Optimizer, and TrainingData
  float learning_rate = 1;
  float beta1 = 1;
  float beta2 = 1;

  Optimizer *optimizer = new AdamOptimizer(learning_rate, beta1, beta2);
  Network network(input_size, hidden_size, output_size, optimizer,
                  learning_rate);

  std::cout << "Training..." << std::endl;
  Training training(&network, data_file, optimizer);
  training.Train(num_epochs, output_at_end, 0, to_line);

  std::cout << "Testing..." << std::endl;
  Testing testing(&network, data_file);
  testing.Test(output_at_end, to_line, 0);

  return 0;
}