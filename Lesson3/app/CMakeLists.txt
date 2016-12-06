#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set (CMAKE_C_FLAGS "--std=c99 -g ${CMAKE_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "--std=c++11 -g ${CMAKE_CXX_FLAGS}")
endif()

include_directories(${azure_IoT_Sdks}/c/c-utility/inc
                    ${azure_IoT_Sdks}/c/iothub_client/inc)

link_directories(${azure_IoT_Sdks}/c/cmake/iotsdk_linux/serializer
                 ${azure_IoT_Sdks}/c/cmake/iotsdk_linux/iothub_client
                 ${azure_IoT_Sdks}/c/cmake/iotsdk_linux/c-utility
                 ${azure_IoT_Sdks}/c/cmake/iotsdk_linux/umqtt)

add_executable(lesson3 main.c)
target_link_libraries(lesson3 wiringPi
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