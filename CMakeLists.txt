cmake_minimum_required(VERSION 3.5.0)


file(GLOB I18N_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/i18n/*")
foreach(I18N_FILE ${I18N_FILES})
    get_filename_component(FILENAME ${I18N_FILE} NAME)
    configure_file(${I18N_FILE}
                   ${CMAKE_CURRENT_BINARY_DIR}/i18n/${FILENAME}
                   COPYONLY)
endforeach()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/smlp.conf
               ${CMAKE_CURRENT_BINARY_DIR}/smlp.conf
               COPYONLY)

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
        COMMAND lcov --capture --directory . --output-file ../coverage/lcov.info
        COMMAND lcov --remove ../coverage/lcov.info --output-file ../coverage/lcov.info '/usr/include/*' '/usr/lib/*' '${CMAKE_CURRENT_SOURCE_DIR}/libs/csv-parser/*' '${CMAKE_CURRENT_SOURCE_DIR}/libs/json/*' '${CMAKE_CURRENT_SOURCE_DIR}/test/*' '${CMAKE_CURRENT_SOURCE_DIR}/src/*'
        COMMENT "Generating lcov.info"
    )
    add_custom_target(lcov_summary
        COMMAND lcov --summary ../coverage/lcov.info
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


enable_testing()
add_subdirectory(test)