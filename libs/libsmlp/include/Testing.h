#include "Network.h"
#include <string>

class Testing {
public:
  Testing(Network *network, const std::string &test_data_file_path)
      : network_(network), test_data_file_path_(test_data_file_path) {}

  int Test();

private:
  Network *network_;
  std::string test_data_file_path_;
};