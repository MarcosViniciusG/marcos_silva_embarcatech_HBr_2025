cmake_minimum_required(VERSION 3.13)
project(unit_tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories(../include)

add_executable(run_tests
    test_adc_to_celsius.c
    ../include/temperature.c
    ../include/unity.c
)

enable_testing()
add_test(NAME adc_to_celsius COMMAND run_tests)