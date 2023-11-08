#include "Common.h"
#include "HiddenLayer.h"
#include "Network.h"
#include "doctest.h"

TEST_CASE("Testing the Network class") {
  SUBCASE("Test Constructor and Destructor") {
    Network *network = nullptr;
    CHECK_NOTHROW(network = new Network());
    CHECK_NOTHROW(delete network);
    CHECK_NOTHROW(network = new Network({.input_size = 10,
                                         .hidden_size = 3,
                                         .output_size = 2,
                                         .hiddens_count = 2}));
    CHECK_NOTHROW(delete network);
  }

  SUBCASE("Test Activation functions") {
    auto network = new Network();
    auto hlayer = new HiddenLayer();
    Neuron n1;
    Neuron n2;
    hlayer->neurons.push_back(n1);
    hlayer->neurons.push_back(n2);
    bool hasElu = false;
    bool hasTanh = false;

    for (auto activ :
         {EActivationFunction::ELU, EActivationFunction::LReLU,
          EActivationFunction::PReLU, EActivationFunction::ReLU,
          EActivationFunction::Sigmoid, EActivationFunction::Tanh}) {
      CHECK_NOTHROW(network->SetActivationFunction(hlayer, activ, 0.1f));
      for (auto neu : hlayer->neurons) {
        if (activ == EActivationFunction::ELU) {
          CHECK(neu.activationFunction(1.0f) == 1.0);
          CHECK(neu.activationFunction(-1.0f) ==
                doctest::Approx(-0.06321).epsilon(0.0001));
          CHECK(neu.activationFunction(0.0f) == 0.0f);
          CHECK(neu.activationFunctionDerivative(1.0f) == 1.0);
          CHECK(neu.activationFunctionDerivative(-1.0f) ==
                doctest::Approx(0.0367879f).epsilon(0.0001));
          hasElu = true;
        } else if (activ == EActivationFunction::Tanh) {
          CHECK(neu.activationFunction(0.0f) ==
                doctest::Approx(0.0).epsilon(0.0001));
          CHECK(neu.activationFunction(1.0f) ==
                doctest::Approx(0.7615941559557649).epsilon(0.0001));
          CHECK(neu.activationFunction(-1.0f) ==
                doctest::Approx(-0.7615941559557649).epsilon(0.0001));
          CHECK(neu.activationFunctionDerivative(0.0f) == 1);
          CHECK(neu.activationFunctionDerivative(1.0f) ==
                doctest::Approx(0.41997434161402614).epsilon(0.0001));
          CHECK(neu.activationFunctionDerivative(-1.0f) ==
                doctest::Approx(0.41997434161402614).epsilon(0.0001));
          hasTanh = true;
        }
      }
    }
    CHECK(hasElu == true);
    CHECK(hasTanh == true);

    CHECK_NOTHROW(delete hlayer);
    CHECK_NOTHROW(delete network);
  }
}