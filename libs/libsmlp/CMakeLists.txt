cmake_minimum_required(VERSION 3.0.0)

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

target_include_directories(${LIBRARY_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(test)

enable_testing()