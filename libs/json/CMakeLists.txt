cmake_minimum_required(VERSION 3.5.0)

option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" TRUE)

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage")    
    set(CMAKE_EXE_LINKER_FLAGS "-lgcov")
endif()


set(PROJECT_NAME nlohmann-json)
set(LIBRARY_NAME nlohmann-json)
set(LIBRARY_HEADERS_DIR include)
file(GLOB LIBRARY_HEADER_FILES ${LIBRARY_HEADERS_DIR}/**.hpp)
set(LIBRARY_HEADERS ${LIBRARY_HEADERS_FILES})

project(${PROJECT_NAME})
add_library(${LIBRARY_NAME} INTERFACE)
target_sources(${LIBRARY_NAME}
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_HEADERS_DIR}/json.hpp
)
target_include_directories(${LIBRARY_NAME}
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_HEADERS_DIR}
)