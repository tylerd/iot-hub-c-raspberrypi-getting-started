/*
* IoT Hub Raspberry Pi C Blink - Microsoft Sample Code - Copyright (c) 2016 - Licensed MIT
*/

#include <stdio.h>
#include <stdbool.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define MAX_BLINK_TIMES 20
#define RED_LED_PIN 7

// TRIGGERING TEST COMMIT -XXYAABB
int main(int argc, char *argv[])
{
    int blinkNumber = 0;
    
    wiringPiSetup();

    while (MAX_BLINK_TIMES > blinkNumber++)
    {
        printf("[Device] #%d Blink LED \n", blinkNumber);
        digitalWrite(RED_LED_PIN, HIGH);
        delay(100);
        digitalWrite(RED_LED_PIN, LOW);
        delay(2000);
    }

    return 0;
}
