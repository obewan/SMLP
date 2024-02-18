configure_file(${CMAKE_CURRENT_SOURCE_DIR}/smlp.conf
              ${CMAKE_BINARY_DIR}/test/ctests/smlp.conf
              COPYONLY)

#### CTEST - INTEGRATION TESTS
include(CTest) 
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
# A macro to simplify the syntax
macro(add_my_test TEST_NAME COMMAND_NAME)
  add_test(NAME ${TEST_NAME} 
           COMMAND ${COMMAND_NAME} ${ARGN}
           WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endmacro()
# Rename the config file to a temporary file
add_test(NAME rename_config_file
         COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/smlp.conf ${CMAKE_BINARY_DIR}/smlp.conf.tmp)
# Test version
add_my_test(test_version smlp -v)
set_tests_properties(test_version PROPERTIES
  PASS_REGULAR_EXPRESSION "SMLP - Simple Multilayer Perceptron v"
)
# Test help
add_my_test(test_help smlp -h)
set_tests_properties(test_help PROPERTIES
  PASS_REGULAR_EXPRESSION "Usage: .*smlp \\[OPTIONS\\]"
)
# Test trainonly
add_my_test(test_trainonly smlp -e ${CMAKE_BINARY_DIR}/ctestModel.json -f ${CMAKE_BINARY_DIR}/../data/mushroom/mushroom_data.csv -s 20 -o 1 -d 12 -c 1 -p 2 -a ReLU -m TrainOnly -x)
set_tests_properties(test_trainonly PROPERTIES
  PASS_REGULAR_EXPRESSION "InputSize:20 OutputSize:1 HiddenSize:12 HiddenLayers:1 LearningRate:0.01 HiddenActivationFunction:ReLU OutputActivationFunction:Sigmoid Epochs:2 TrainingRatio:0.7 Mode:TrainOnly Verbose:0"
)
set_tests_properties(test_trainonly PROPERTIES
  PASS_REGULAR_EXPRESSION "Training epoch 1/2..."
)
set_tests_properties(test_trainonly PROPERTIES
  PASS_REGULAR_EXPRESSION "Training epoch 2/2..."
)
# Test previous model file exists
add_my_test(check_ctestModelJson_exists ${CMAKE_COMMAND} -E cat ${CMAKE_BINARY_DIR}/ctestModel.json)
# Test testonly
add_my_test(test_testonly smlp -i ${CMAKE_BINARY_DIR}/ctestModel.json -f  ${CMAKE_BINARY_DIR}/../data/mushroom/mushroom_data.csv -m TestOnly -x)
set_tests_properties(test_testonly PROPERTIES
  PASS_REGULAR_EXPRESSION "Importing network model from .*ctestModel.json"
)
set_tests_properties(test_testonly PROPERTIES
  PASS_REGULAR_EXPRESSION "Testing, using file .*mushroom_data.csv"
)
# Test predict
add_my_test(test_predict smlp -i ${CMAKE_BINARY_DIR}/ctestModel.json -f ${CMAKE_BINARY_DIR}/../data/mushroom/mushroom_data_to_predict.csv -m Predict -x)
set_tests_properties(test_predict PROPERTIES
  PASS_REGULAR_EXPRESSION "[01],0.08,0.43,0.9,0.42,1,0.62,0.33,0.38,0.1,0.07,0,0,0.38,0,0,1,0.92,0,1,0"
)
set_tests_properties(test_predict PROPERTIES
  PASS_REGULAR_EXPRESSION "[01],0.01,0.57,0.9,0.25,1,0,0.67,0.92,0.09,0.02,0,0,0.62,0,0,1,0.92,0,1,0"
)
set_tests_properties(test_predict PROPERTIES
  PASS_REGULAR_EXPRESSION "[01],0.06,0.43,0,0.75,1,0.12,0.33,0.62,0.11,0.09,0,0,0.23,0,0,1,0.92,0,1,0"
)
# Test config 
add_my_test(check_smlpConfig_create ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/test/ctests/smlp.conf ${CMAKE_BINARY_DIR}/smlp.conf)
add_my_test(check_smlpConfig_exists ${CMAKE_COMMAND} -E cat ${CMAKE_BINARY_DIR}/smlp.conf)
add_my_test(test_trainonly_withconfig smlp -s 20 -o 1 -d 12 -c 1 -p 1 -a ReLU -m TrainOnly -x)
set_tests_properties(test_trainonly_withconfig PROPERTIES
PASS_REGULAR_EXPRESSION "Using config file .*smlp.conf..."
)
set_tests_properties(test_trainonly_withconfig PROPERTIES
PASS_REGULAR_EXPRESSION "InputSize:20 OutputSize:1 HiddenSize:12 HiddenLayers:1 LearningRate:0.01 HiddenActivationFunction:ReLU OutputActivationFunction:Sigmoid Epochs:1 TrainingRatio:0.7 Mode:TrainOnly Verbose:0"
)
# Restore the config file
add_test(NAME restore_config_file
         COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/smlp.conf.tmp ${CMAKE_BINARY_DIR}/smlp.conf)
