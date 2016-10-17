---
services: iot-hub, iot-suite
platforms: c
author: zikalino
---

# Configure a Rapberry Pi 3 device and run a sample app on it
This sample accompanies [Get started with your Raspberry Pi 3](https://aka.ms/rpi-c) tutorials. It shows how to setup a Raspberry Pi 3
device, install an OS, and deploy and run a sample application.

## Repository information
- `app` sub-folder contains the sample C application that blinks the LED attached to the Pi.
- `config.json` contains required configuration settings.

## Running this sample
Please follow the [step-by-step instructions](https://aka.ms/rpi-c-1) for detailed walkthough of the steps below.

### Install required tools

```bash
npm install -g device-discovery-cli gulp
```

### Deploy and run

```bash
npm install
```

```bash
gulp install-tools
gulp build
gulp deploy
gulp run
```
