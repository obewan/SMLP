#include "Common.h"
#include "HiddenLayer.h"
#include "Network.h"
#include "SimpleLang.h"
#include "doctest.h"
#include "exception/NetworkException.h"

TEST_CASE("Testing the Network class") {
  const float eps = 1e-6f; // epsilon for float testing

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
      for (const auto &neu : hlayer->neurons) {
        if (activ == EActivationFunction::ELU) {
          CHECK(neu.activationFunction(1.0f) == 1.0);
          CHECK(neu.activationFunction(-1.0f) ==
                doctest::Approx(-0.0632121).epsilon(eps));
          CHECK(neu.activationFunction(0.0f) == 0.0f);
          CHECK(neu.activationFunctionDerivative(1.0f) == 1.0);
          CHECK(neu.activationFunctionDerivative(-1.0f) ==
                doctest::Approx(0.0367879f).epsilon(eps));
          hasElu = true;
        } else if (activ == EActivationFunction::Tanh) {
          CHECK(neu.activationFunction(0.0f) ==
                doctest::Approx(0.0).epsilon(eps));
          CHECK(neu.activationFunction(1.0f) ==
                doctest::Approx(0.7615941559557649).epsilon(eps));
          CHECK(neu.activationFunction(-1.0f) ==
                doctest::Approx(-0.7615941559557649).epsilon(eps));
          CHECK(neu.activationFunctionDerivative(0.0f) == 1);
          CHECK(neu.activationFunctionDerivative(1.0f) ==
                doctest::Approx(0.41997434161402614).epsilon(eps));
          CHECK(neu.activationFunctionDerivative(-1.0f) ==
                doctest::Approx(0.41997434161402614).epsilon(eps));
          hasTanh = true;
        }
      }
    }
    CHECK(hasElu == true);
    CHECK(hasTanh == true);

    auto invalidEnum = static_cast<EActivationFunction>(900);
    CHECK_THROWS_WITH_AS(
        network->SetActivationFunction(hlayer, invalidEnum, 0.1f),
        SimpleLang::Error(Error::UnimplementedActivationFunction).c_str(),
        NetworkException);

    CHECK_NOTHROW(delete hlayer);
    CHECK_NOTHROW(delete network);
  }
}