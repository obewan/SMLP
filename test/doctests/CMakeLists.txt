#### DOCTEST - UNIT TESTS
option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" TRUE)


if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -pthread")
endif()

if(ENABLE_COVERAGE AND NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage")    
    set(CMAKE_EXE_LINKER_FLAGS "-lgcov")
endif()

target_sources(nlohmann-json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/json/include/json.hpp)
add_library(lib_doctest INTERFACE)
target_sources(lib_doctest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/doctest.h)
set(LIBRARY_HEADERS_DIR include)
target_include_directories(lib_doctest INTERFACE  "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_HEADERS_DIR}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../libs/libsmlp/include)


file(GLOB SOURCES "*.cpp")
add_executable(tester ${SOURCES}) 
target_link_libraries(tester PRIVATE ${LIBRARY_NAME} lib_doctest nlohmann-json libsmlp)

