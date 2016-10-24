/*
* IoT Hub Raspberry Pi C Blink - Microsoft Sample Code - Copyright (c) 2016 - Licensed MIT
*/

#include <stdio.h>
#include <stdbool.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define RED_LED_PIN 7

int main(int argc, char *argv[])
{
    bool isLedOn = false;
    
    wiringPiSetup();

    for (;;)
    {
        printf("[Device] Switching %s the LED...\n", isLedOn ? "on" : "off");
        digitalWrite(RED_LED_PIN, isLedOn ? HIGH : LOW);
        isLedOn = !isLedOn;
        delay(2000);
    }
}
