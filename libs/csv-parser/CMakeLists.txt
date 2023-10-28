cmake_minimum_required(VERSION 3.5.0)

option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" TRUE)

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage")    
    set(CMAKE_EXE_LINKER_FLAGS "-lgcov")
endif()


set(PROJECT_NAME csv-parser)
set(LIBRARY_NAME csv-parser)
set(LIBRARY_HEADERS_DIR include)
file(GLOB LIBRARY_HEADER_FILES ${LIBRARY_HEADERS_DIR}/**.h)
set(LIBRARY_HEADERS ${LIBRARY_HEADERS_FILES})

project(${PROJECT_NAME})
add_library(${LIBRARY_NAME} INTERFACE)
target_sources(${LIBRARY_NAME}
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_HEADERS_DIR}/csv_cell.h
		${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_HEADERS_DIR}/csv_error.h
		${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_HEADERS_DIR}/csv_parser.h
)
target_include_directories(${LIBRARY_NAME}
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_HEADERS_DIR}
)