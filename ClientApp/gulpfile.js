/*
* IoT Hub Raspberry Pi C - Microsoft Sample Code - Copyright (c) 2016 - Licensed MIT
*/
'use strict';

/**
 * Setup common gulp tasks: init, install-tools, deploy, run
 */
require('gulp-common')(require('gulp'), 'raspberrypi-c-on-device', {
  appName: 'client-app',
  configTemplate: {
    "device_host_name_or_ip_address": "[device hostname or IP adress]",
    "device_user_name": "pi",
    "device_password": "raspberry",
    "iot_device_connection_string": "[IoT device connection string]"
  },
  configPostfix: "raspberrypi",
  app: [ 'main.c', 'bme280.h', 'bme280.c', 'CMakeLists.txt' ]
});
