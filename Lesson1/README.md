---
services: iot-hub, iot-suite
platforms: C
author: zikalino
---

# Configure a Rapberry Pi 3 device and run a sample app on it
This sample accompanies [Get started with your Raspberry Pi 3](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-raspberry-pi-kit-c-lesson1-configure-your-device/) lesson. It shows how to setup a Raspberry Pi 3 device, install an OS, and deploy and run a sample application.
device, install an OS, and deploy and run a sample application.

## Repository information
- `app` sub-folder contains the sample C application that blinks the LED attached to the Pi.
- `config.json` contains required configuration settings.

## Running this sample
Please follow the [Lesson 1: Configure your device](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-raspberry-pi-kit-c-lesson1-configure-your-device/) for detailed walkthough of the steps below.

### Install required tools

```bash
npm install -g device-discovery-cli gulp
```

### Deploy and run

Install required npm packages on the host:
```bash
npm install
```
Create a JSON configuration file in the `.iot-hub-getting-started` sub-folder of the current user's home directory:
```bash
gulp init
```

Install required tools/packages on the Raspberry Pi 3 device, deploy sample application, and run it on the device:
```bash
gulp install-tools
gulp deploy
gulp run
```
