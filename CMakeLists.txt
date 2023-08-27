cmake_minimum_required(VERSION 3.0.0)

set(PROJECT_NAME smlp)
set(SOURCE_DIR src)
set(SOURCE ${SOURCE_DIR}/SimpleMLP.cpp)
set(LIBRARIES_DIR libs)
set(LIBRARIES libsmlp csv-parser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${SOURCE})

foreach(LIBRARY ${LIBRARIES})
    add_subdirectory("${LIBRARIES_DIR}/${LIBRARY}")
endforeach(LIBRARY)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
