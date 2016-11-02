/*
* IoT Hub Raspberry Pi C Azure Blink - Microsoft Sample Code - Copyright (c) 2016 - Licensed MIT
*/

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothub_client.h"
#include "iothub_message.h"
#include "iothubtransportamqp.h"

#define MAX_BLINK_TIMES 20

const int RED_LED_PIN = 7;
int totalBlinkTimes = 1;
int lastMessageSentTime = 0;
bool messagePending = false;

static void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    if (IOTHUB_CLIENT_CONFIRMATION_OK == result)
    {
        digitalWrite(RED_LED_PIN, HIGH);
        delay(100);
        digitalWrite(RED_LED_PIN, LOW);
    }
    else
    {
        printf("[Device] Failed to send message to Azure IoT Hub\r\n");
    }

    messagePending = false;
}

static void sendMessageAndBlink(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    char buffer[256];
    sprintf(buffer, "{ deviceId: %s, messageId: %d }", "myraspberrypi", totalBlinkTimes);

    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, strlen(buffer));
    if (messageHandle == NULL)
    {
        printf("[Device] unable to create a new IoTHubMessage\r\n");
    }
    else
    {
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            printf("[Device] Failed to hand over the message to IoTHubClient\r\n");
        }
        else
        {
            lastMessageSentTime = millis();
            messagePending = true;
            printf("[Device] Sending message #%d: %s\r\n", totalBlinkTimes, buffer);
        }

        IoTHubMessage_Destroy(messageHandle);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("[Device] IoT Hub connection string should be passed as a parameter\r\n");
        return 1;
    }

    wiringPiSetup();
    pinMode(RED_LED_PIN, OUTPUT);

    IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

    (void)printf("[Device] Starting the IoTHub client sample AMQP...\r\n");

    if (platform_init() != 0)
    {
        printf("[Device] Failed to initialize the platform.\r\n");
    }
    else
    {
        if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(argv[1], AMQP_Protocol)) == NULL)
        {
            (void)printf("[Device] ERROR: iotHubClientHandle is NULL!\r\n");
        }
        else
        {
            while ((totalBlinkTimes <= MAX_BLINK_TIMES) || messagePending)
            {
                if ((lastMessageSentTime + 2000 < millis()) && !messagePending)
                {
                    sendMessageAndBlink(iotHubClientHandle);
                    totalBlinkTimes++;
                }

                IoTHubClient_LL_DoWork(iotHubClientHandle);            
                delay(100);
            } 

            IoTHubClient_LL_Destroy(iotHubClientHandle);
        }
        platform_deinit();
    }

    return 0;
}
