add_library(lib_doctest INTERFACE)
target_sources(lib_doctest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/doctest.h)
set(LIBRARY_HEADERS_DIR include)
target_include_directories(lib_doctest INTERFACE  "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_HEADERS_DIR}")

file(GLOB SOURCES "*.cpp")
add_executable(tester ${SOURCES}) 
target_link_libraries(tester PRIVATE ${LIBRARY_NAME} lib_doctest)