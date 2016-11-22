#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set (CMAKE_C_FLAGS "--std=c99 -g ${CMAKE_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "--std=c++11 -g ${CMAKE_CXX_FLAGS}")
endif()

add_executable(app main.c)
target_link_libraries(app wiringPi)
