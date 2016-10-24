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
#include "jsondecoder.h"

#include "config.h"

const int RED_LED_PIN = 7;

static void blinkLED()
{
    digitalWrite(RED_LED_PIN, HIGH);
    delay(100);
    digitalWrite(RED_LED_PIN, LOW);
}

IOTHUBMESSAGE_DISPOSITION_RESULT receiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    const unsigned char* buffer = NULL;
    size_t size = 0;

    if (IOTHUB_MESSAGE_OK != IoTHubMessage_GetByteArray(message, &buffer, &size))
        return IOTHUBMESSAGE_ABANDONED;

    // message needs to be converted to zero terminated string
    char* s = malloc(size + 1);

    if (NULL == s)
        return IOTHUBMESSAGE_ABANDONED;

    strncpy(s, buffer, size);
    s[size] = 0;

    printf("[Device] Received message: %s\r\n", s);

    MULTITREE_HANDLE tree = NULL;

    if (JSON_DECODER_OK == JSONDecoder_JSON_To_MultiTree(s, &tree))
    {
        const void* value = NULL;

        if (MULTITREE_OK == MultiTree_GetLeafValue(tree, "/command", &value))
        {
            if (0 == strcmp((const char*)value, "\"blink\""))
            {
                blinkLED();
            }
        }
    }

    free(s);
    MultiTree_Destroy(tree);
    
    return IOTHUBMESSAGE_ACCEPTED;
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
            IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL);

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
