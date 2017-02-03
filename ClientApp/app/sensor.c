#include "sensor.h"

#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <wiringPi.h>

bool has_setup = false;
void setup(){
    if(has_setup) return;
    wiringPiSetup();
    has_setup = true;
}

Sensor_Data * read_sensor_data_with_retry()
{
    Sensor_Data *data = NULL;
    for(int i = 0; i < RETRY_COUNT; i++)
    {
        data = read_sensor_data();
        if(data != NULL)
        {
            break;
        }
    }
    return data;
}


Sensor_Data *read_sensor_data()
{
    setup();
    int data[5];
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    pinMode(PIN, OUTPUT);
    digitalWrite(PIN, HIGH);
    delay(80);
    digitalWrite(PIN, LOW);
    delay(10);    

    digitalWrite(PIN, HIGH);
    delayMicroseconds(40);

    pinMode(PIN, INPUT);
    pullUpDnControl(PIN, PUD_UP);
    delayMicroseconds(10);

    if(pulse(LOW) == 0 || pulse(HIGH) == 0) 
    {
        return NULL;
    }

    int cycles[80];
    int i;
    for (i = 0; i < 80; i+=2) {
        cycles[i] = pulse(LOW);
        cycles[i + 1] = pulse(HIGH);
    }

    for(i = 0; i < 40; i++) {
        int low = cycles[2 * i];
        int high = cycles[2 * i + 1];

        if(low == 0 || high == 0) {
            return NULL;
        }
        data[i/8] <<= 1;
        if(high > low) {
            data[i/8] |= 1;
        }
    }

    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return NULL;
    }

    Sensor_Data *sensor = (Sensor_Data *)malloc(sizeof(Sensor_Data));
    if(sensor == NULL)
    {
        return NULL;
    }

    sensor -> temperature = ((data[2] & 0x7F) * 256 + data[3]) * 0.1;
    sensor -> humidity = (data[0] * 256 + data[1]) * 0.1;
    return sensor;
}


int pulse(int value)
{
    int timer = 0;
    while(digitalRead(PIN) == value) {
        timer++;
        delayMicroseconds(1);
        if(timer == TIMEOUT) {
            break;
        }
    }
    if(timer == TIMEOUT) {
        return 0;
    }
    return timer;
}