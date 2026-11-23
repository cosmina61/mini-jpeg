add_executable(unit_tests test_smoke.cpp)
add_test(NAME smoke COMMAND unit_tests)

add_executable(test_color test_color.cpp)
target_link_libraries(test_color PRIVATE mjpeg_core)
add_test(NAME color COMMAND test_color)