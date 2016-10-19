/*
* IoT Hub Raspberry Pi C - Microsoft Sample Code - Copyright (c) 2016 - Licensed MIT
*/
'use strict';

/**
 * Setup common gulp tasks: init, install-tools, deploy, run
 */
require('gulp-common')(require('gulp'), 'raspberrypi-c-on-device', {
  appName: 'lesson-1',
  configTemplate: {
    "device_host_name_or_ip_address": "[device hostname or IP adress]",
    "device_user_name": "pi",
    "device_password": "raspberry"
  },
  configPostfix: "raspberrypi",
  app: [ 'main.c' ],
  inc: [],
  lib: [ 'wiringPi' ]
});
