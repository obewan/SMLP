cmake_minimum_required(VERSION 3.5.0)

option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" TRUE)

if(ENABLE_COVERAGE AND NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage")    
    set(CMAKE_EXE_LINKER_FLAGS "-lgcov") 
endif()

# Add the csv-parser and json libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../csv-parser csv-parser)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../json json)

set(PROJECT_NAME libsmlp)
set(LIBRARY_NAME libsmlp)
set(LIBRARY_HEADERS_DIR include)
file(GLOB LIBRARY_HEADER_FILES ${LIBRARY_HEADERS_DIR}/**.h)
set(LIBRARY_HEADERS ${LIBRARY_HEADERS_FILES})

set(LIBRARY_SOURCE_DIR src)
file(GLOB LIBRARY_SOURCE_FILES ${LIBRARY_SOURCE_DIR}/**.cpp)
set(LIBRARY_SOURCE ${LIBRARY_SOURCE_FILES})

project(${PROJECT_NAME})

add_library(${LIBRARY_NAME} STATIC
    ${LIBRARY_HEADERS}
    ${LIBRARY_SOURCE}
)

# Link the csv-parser and json libraries
#target_link_libraries(${LIBRARY_NAME} PUBLIC csv-parser json)

target_include_directories(${LIBRARY_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../csv-parser/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../json/include>
    $<INSTALL_INTERFACE:include>
)
