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

#include "config.h"

#define MAX_BLINK_TIMES 20

const int RED_LED_PIN = 7;
int totalBlinkTimes = 1;
int lastMessageSentTime = 0;
bool messagePending = false;

static void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    if (IOTHUB_CLIENT_CONFIRMATION_OK == result)
    {
        printf("[Device] Message sent to Azure IoT Hub\r\n");
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

IOTHUBMESSAGE_DISPOSITION_RESULT messageHandler(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    printf("[Device] Message received\r\n");
}

int main(void)
{
    wiringPiSetup();

    IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

    (void)printf("[Device] Starting the IoTHub client sample AMQP...\r\n");

    if (platform_init() != 0)
    {
        printf("[Device] Failed to initialize the platform.\r\n");
    }
    else
    {
        if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, AMQP_Protocol)) == NULL)
        {
            (void)printf("[Device] ERROR: iotHubClientHandle is NULL!\r\n");
        }
        else
        {
            IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, messageHandler, NULL);

            while (true)
            {
                IoTHubClient_LL_DoWork(iotHubClientHandle);            
                delay(100);
            } 

            IoTHubClient_LL_Destroy(iotHubClientHandle);
        }
        platform_deinit();
    }

    return 0;
}
