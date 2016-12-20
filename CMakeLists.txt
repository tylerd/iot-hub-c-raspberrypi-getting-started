cmake_minimum_required(VERSION 3.5)
project(iothub_c_raspberrypi_gettingstarted)

option(compileOption_C "passes a string to the command line of the C compiler" OFF)
option(azure_IoT_Sdk_c "passes path of azure iot sdk c build libs source path" OFF)

set (CMAKE_C_FLAGS "${compileOption_C} ${CMAKE_C_FLAGS}")

add_subdirectory(Lesson1/app)
add_subdirectory(Lesson3/app)
add_subdirectory(Lesson4/app)