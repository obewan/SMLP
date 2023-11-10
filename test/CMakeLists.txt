configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/test_file.csv
               ${CMAKE_CURRENT_BINARY_DIR}/data/test_file.csv
               COPYONLY)


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
# Test version
add_test(NAME test_version COMMAND smlp -v WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(test_version PROPERTIES
  PASS_REGULAR_EXPRESSION "SMLP - Simple Multilayer Perceptron v"
)
# Test help
add_test(NAME test_help COMMAND smlp -h WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(test_help PROPERTIES
  PASS_REGULAR_EXPRESSION "Usage: .*smlp \\[OPTIONS\\]"
)
# Test trainonly
add_test(NAME test_trainonly COMMAND smlp -b ctestModel.json -f ../data/mushroom/mushroom_data.csv -i 20 -o 1 -d 12 -c 1 -e 2 -j ReLU -m TrainOnly WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(test_trainonly PROPERTIES
  PASS_REGULAR_EXPRESSION "InputSize:20 OutputSize:1 HiddenSize:12 HiddenLayers:1 LearningRate:0.01 HiddenActivationFunction:ReLU OutputActivationFunction:Sigmoid Epochs:2 TrainingRatio:0.7 Mode:TrainOnly Verbose:0"
)
set_tests_properties(test_trainonly PROPERTIES
  PASS_REGULAR_EXPRESSION "Training epoch 1/2..."
)
set_tests_properties(test_trainonly PROPERTIES
  PASS_REGULAR_EXPRESSION "Training epoch 2/2..."
)
add_test(NAME check_ctestModelJson_exists COMMAND ${CMAKE_COMMAND} -E cat ${CMAKE_BINARY_DIR}/ctestModel.json)
# Test testonly
add_test(NAME test_testonly COMMAND smlp -a ctestModel.json -f ../data/mushroom/mushroom_data.csv -m TestOnly WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(test_testonly PROPERTIES
  PASS_REGULAR_EXPRESSION "Importing network model from ctestModel.json"
)
set_tests_properties(test_testonly PROPERTIES
  PASS_REGULAR_EXPRESSION "Testing, using file ../data/mushroom/mushroom_data.csv"
)
# Test predict
add_test(NAME test_predictive COMMAND smlp -a ctestModel.json -f ../data/mushroom/mushroom_data_to_predict.csv -m Predictive WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(test_testonly PROPERTIES
  PASS_REGULAR_EXPRESSION "1,0.08,0.43,0.9,0.42,1,0.62,0.33,0.38,0.1,0.07,0,0,0.38,0,0,1,0.92,0,1,0"
)
set_tests_properties(test_testonly PROPERTIES
  PASS_REGULAR_EXPRESSION "0,0.01,0.57,0.9,0.25,1,0,0.67,0.92,0.09,0.02,0,0,0.62,0,0,1,0.92,0,1,0"
)
set_tests_properties(test_testonly PROPERTIES
  PASS_REGULAR_EXPRESSION "0,0.06,0.43,0,0.75,1,0.12,0.33,0.62,0.11,0.09,0,0,0.23,0,0,1,0.92,0,1,0"
)