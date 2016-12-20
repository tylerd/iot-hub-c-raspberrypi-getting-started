# Build Microsoft Azure IoT Raspberry Pi Starter Kit On Ubuntu
To build Azure IoT Raspberry Pi Starter Kit C On Ubuntu, you'll need build azure iot sdk firstly. Pls see below instruction for detail.

## Environment setup

## Build steps
1. Cross compile azure iot sdks on C. Below steps are copied from [Cross Compiling the Azure IoT Hub SDK C](https://github.com/Azure/azure-iot-sdk-c/blob/master/doc/SDK_cross_compile_example.md), pls refer to original source for more detail.
a. Get required Raspberry Pi toolchain, system libraries and system headers from https://github.com/raspberrypi/tools. For example:
 ```bash
cd ~
mkdir RPiTools
cd RPiTools
git clone https://github.com/raspberrypi/tools.git
```

b. Copy some files from a running Raspberry Pi to your host machine.
```bash
cd ~/RPiTools/tools/arm-bcm2708/\
gcc-linaro-arm-linux-gnueabihf-raspbian-x64/arm-linux-gnueabihf
rsync -rl --safe-links pi@<your Pi identifier>:/{lib,usr} .
```
In the above command replace &lt;*your Pi identifier*&gt; with the IP address of your Raspberry Pi.

c. Setup cmake to cross compile. In same directory as above enter the following command
```bash
export RPI_ROOT=$(pwd)
```
Then create toolchain-rpi.cmake file, Enter following commands
```bash
cd ~/Source/azure-iot-sdk-c/build_all/linux
```
```cmake
INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Linux)     # this one is important
SET(CMAKE_SYSTEM_VERSION 1)     # this one not so much

# this is the location of the amd64 toolchain targeting the Raspberry Pi
SET(CMAKE_C_COMPILER $ENV{RPI_ROOT}/../bin/arm-linux-gnueabihf-gcc)

# this is the file system root of the target
SET(CMAKE_FIND_ROOT_PATH $ENV{RPI_ROOT})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```
and save the toolchain file. Your cross compilation environment is now complete.

d. Build aure iot sdks
Build with following commands
```bash
cd ~/Source/azure-iot-sdk-c/build_all/linux
./build.sh --toolchain-file toolchain-rpi.cmake -cl --sysroot=$RPI_ROOT
```
This will tell cmake to build the SDK using the toolchain file toolchain-rpi.cmake and skip running all tests which is important since the executables will (probably) not run successfully on the host anyway. Finally, and absolutely critical is the use of the *--sysroot* option. Without this the compiler will fail to find required headers and libraries.



2. Cross compile raspberry pi starter kit on C. After build azure iot sdks on C, run below commands to build raspberry pi started kit on C.
```
cd ~/Source/iot-hub-c/raspberrypi-getting-started/.misc
./build.sh --toolchain-file ./toolchain-rpi.cmake -cl --sysroot=$RPI_ROOT --azure-iot-sdk-c=<your azure iot sdk c root path built at step 1>
```

## References
<https://github.com/Azure/azure-iot-sdks>

<https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md>

<https://github.com/raspberrypi/tools>

<https://cmake.org/Wiki/CMake_Cross_Compiling>
