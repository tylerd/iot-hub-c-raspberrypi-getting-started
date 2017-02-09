---
services: iot-hub, iot-suite
platforms: C
author: yuwzho
---

# Configure a Raspberry Pi 3 device and run a sample app on it
This sample accompanies [Get started with your Raspberry Pi 3](#) lesson. It shows how to set up Raspberry Pi to enable communication with a BME280 sensor. Then deploy and run a sample application on Pi to collect temperature and humidity data from the sensor. Finally send the data collected to your IoT hub.

Please follow the [Lesson 5: Run an application on Raspberry Pi to collect sensor data and send it to Azure IoT Hub](#) for detailed walkthrough of the steps below.

## Repository information
- `app` sub-folder contains the sample C application that blinks the LED attached to the Pi.
- `config.json` contains required configuration settings. The `args` field will be passed to `app/main.c`'s program entry as arguments.

## Connect the BME280 sensor to Pi
### Enable SPI on Pi
Use ssh login to your PI. Then open and edit the file `/boot/config.txt`. Find the following line:

```
#dtparam=spi=on
```

delete the `#` at the begin to uncommenct the line. Then save it and reboot your Pi.


### Connect the sensor to Pi
Connect your BME280 sensor to your Pi according to the following diagram.
![Connect the sensor to your PI](#)

### Don't have a sensor
Open the `config.json`, change the fourth line of code to the following:

```
"args": [5000, true],
```

## Running this sample
### Deploy and run

Install required npm packages on the host:
```bash
npm install
```
Create a JSON configuration file in the `.iot-hub-getting-started` sub-folder of the current user's home directory:
```bash
gulp init
```

Edit the `~/.iot-hub-getting-started/config-raspberrypi.json` file, fill in your Raspberry Pi's IP address and IoT device connection string.

Install required tools/packages on the Raspberry Pi 3 device, deploy sample application, and run it on the device:
```bash
gulp install-tools
gulp deploy
gulp run
```
