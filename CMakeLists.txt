cmake_minimum_required(VERSION 3.5.0)

option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" TRUE)

set(PROJECT_NAME smlp)
set(SOURCE_DIR src)
set(SOURCE ${SOURCE_DIR}/SimpleMLP.cpp ${SOURCE_DIR}/Main.cpp)
set(LIBRARIES_DIR libs)
set(LIBRARIES libsmlp csv-parser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall")

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage")    
    set(CMAKE_EXE_LINKER_FLAGS "-lgcov")
    add_custom_target(lcov
        COMMAND lcov --capture --directory . --output-file lcov.info
        COMMENT "Generating lcov.info"
    )
    add_custom_target(lcov_summary
        COMMAND lcov --summary lcov.info
        COMMENT "Generating lcov summary"
    )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${SOURCE})

foreach(LIBRARY ${LIBRARIES})
    add_subdirectory("${LIBRARIES_DIR}/${LIBRARY}")
endforeach(LIBRARY)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
