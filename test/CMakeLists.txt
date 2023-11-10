file(GLOB TEST_DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/data/*")

foreach(DATA_FILE ${TEST_DATA_FILES})
    get_filename_component(FILENAME ${DATA_FILE} NAME)
    configure_file(${DATA_FILE}
                   ${CMAKE_CURRENT_BINARY_DIR}/data/${FILENAME}
                   COPYONLY)
endforeach()


option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" TRUE)

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage")    
    set(CMAKE_EXE_LINKER_FLAGS "-lgcov")
endif()

add_library(nlohmann-json INTERFACE)
target_sources(nlohmann-json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/json/include/json.hpp)
add_library(lib_doctest INTERFACE)
target_sources(lib_doctest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/doctest.h)
set(LIBRARY_HEADERS_DIR include)
target_include_directories(lib_doctest INTERFACE  "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_HEADERS_DIR}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libs/libsmlp/include)


#### DOCTEST - UNIT TESTS
file(GLOB SOURCES "*.cpp")
add_executable(tester ${SOURCES}) 
target_link_libraries(tester PRIVATE ${LIBRARY_NAME} lib_doctest nlohmann-json libsmlp)


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
add_my_test(test_trainonly smlp -b ${CMAKE_BINARY_DIR}/ctestModel.json -f ${CMAKE_BINARY_DIR}/../data/mushroom/mushroom_data.csv -i 20 -o 1 -d 12 -c 1 -e 2 -j ReLU -m TrainOnly)
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
add_my_test(test_testonly smlp -a  ${CMAKE_BINARY_DIR}/ctestModel.json -f  ${CMAKE_BINARY_DIR}/../data/mushroom/mushroom_data.csv -m TestOnly)
set_tests_properties(test_testonly PROPERTIES
  PASS_REGULAR_EXPRESSION "Importing network model from .*ctestModel.json"
)
set_tests_properties(test_testonly PROPERTIES
  PASS_REGULAR_EXPRESSION "Testing, using file .*mushroom_data.csv"
)
# Test predict
add_my_test(test_predictive smlp -a ${CMAKE_BINARY_DIR}/ctestModel.json -f ${CMAKE_BINARY_DIR}/../data/mushroom/mushroom_data_to_predict.csv -m Predictive)
set_tests_properties(test_predictive PROPERTIES
  PASS_REGULAR_EXPRESSION "[01],0.08,0.43,0.9,0.42,1,0.62,0.33,0.38,0.1,0.07,0,0,0.38,0,0,1,0.92,0,1,0"
)
set_tests_properties(test_predictive PROPERTIES
  PASS_REGULAR_EXPRESSION "[01],0.01,0.57,0.9,0.25,1,0,0.67,0.92,0.09,0.02,0,0,0.62,0,0,1,0.92,0,1,0"
)
set_tests_properties(test_predictive PROPERTIES
  PASS_REGULAR_EXPRESSION "[01],0.06,0.43,0,0.75,1,0.12,0.33,0.62,0.11,0.09,0,0,0.23,0,0,1,0.92,0,1,0"
)
# Test config 
# Restore the config file
add_test(NAME restore_config_file
         COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/smlp.conf.tmp ${CMAKE_BINARY_DIR}/smlp.conf)
add_my_test(check_smlpConfig_exists ${CMAKE_COMMAND} -E cat ${CMAKE_BINARY_DIR}/smlp.conf)
add_my_test(test_trainonly_withconfig smlp -i 20 -o 1 -d 12 -c 1 -e 1 -j ReLU -m TrainOnly)
set_tests_properties(test_trainonly_withconfig PROPERTIES
  PASS_REGULAR_EXPRESSION "Using config file .*smlp.conf..."
)
set_tests_properties(test_trainonly_withconfig PROPERTIES
  PASS_REGULAR_EXPRESSION "InputSize:20 OutputSize:1 HiddenSize:12 HiddenLayers:1 LearningRate:0.01 HiddenActivationFunction:ReLU OutputActivationFunction:Sigmoid Epochs:1 TrainingRatio:0.7 Mode:TrainOnly Verbose:0"
)
add_my_test(check_modelSmlpConfig_exists ${CMAKE_COMMAND} -E cat ${CMAKE_BINARY_DIR}/mushroom_model.json)
