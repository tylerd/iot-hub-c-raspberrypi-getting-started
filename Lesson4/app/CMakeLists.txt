#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set (CMAKE_C_FLAGS "--std=c99 -g ${CMAKE_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "--std=c++11 -g ${CMAKE_CXX_FLAGS}")
endif()

include_directories(~/azure-iot-sdks/c/c-utility/inc
                    ~/azure-iot-sdks/c/iothub_client/inc
                     ~/azure-iot-sdks/c/serializer/inc)

link_directories(~/azure-iot-sdks/c/cmake/iotsdk_linux/serializer
                 ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client
                 ~/azure-iot-sdks/c/cmake/iotsdk_linux/c-utility
                 ~/azure-iot-sdks/c/cmake/iotsdk_linux/umqtt)

add_executable(app main.c)
target_link_libraries(app wiringPi
                          serializer
                          iothub_client
                          iothub_client_mqtt_transport
                          umqtt
                          aziotsharedutil
                          ssl
                          crypto
                          curl
                          pthread
                          m
                          ssl
                          crypto)
