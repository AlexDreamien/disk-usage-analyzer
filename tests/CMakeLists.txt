add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/doctest)

add_executable(dux_tests
    test_main.cpp
    test_format.cpp
    test_scan.cpp
    test_report.cpp
)
target_link_libraries(dux_tests PRIVATE dux_core doctest)

add_test(NAME dux_tests COMMAND dux_tests)
