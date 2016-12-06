/*
* IoT Hub Raspberry Pi C Azure Blink - Microsoft Sample Code - Copyright (c) 2016 - Licensed MIT
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothub_client.h"
#include "iothub_client_options.h"
#include "iothub_message.h"
#include "iothubtransportmqtt.h"
#include "jsondecoder.h"

const int LED_PIN = 7;

bool lastMessageReceived = false;

static void blinkLED()
{
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
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
            else if (0 == strcmp((const char*)value, "\"stop\""))
            {
                lastMessageReceived = true;
            }
        }
    }

    free(s);
    MultiTree_Destroy(tree);

    return IOTHUBMESSAGE_ACCEPTED;
}

char *get_device_id(char *str)
{
    char *substr = strstr(str, "DeviceId=");

    if (substr == NULL)
        return NULL;

    // skip "DeviceId="
    substr += 9;

    char *semicolon = strstr(substr, ";");
    int length = semicolon == NULL ? strlen(substr) : semicolon - substr;
    char *device_id = calloc(1, length + 1);
    memcpy(device_id, substr, length);
    device_id[length] = '\0';

    return device_id;
}

static char *readFile(char *fileName)
{
    FILE *fp;
    int size;
    char *buffer;

    fp = fopen(fileName, "rb");

    if (fp == NULL)
    {
        printf("[Device] ERROR: File %s doesn't exist!\n", fileName);
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    // Allocate memory for entire content
    buffer = calloc(1, size + 1);

    if (buffer == NULL)
    {
        fclose(fp);
        printf("[Device] ERROR: Failed to allocate memory.\n");
        return NULL;
    }

    // Read the file into the buffer
    if (1 != fread(buffer, size, 1, fp))
    {
        fclose(fp);
        free(buffer);
        printf("[Device] ERROR: Failed to read the file %s into memory.\n", fileName);
        return NULL;
    }

    fclose(fp);

    return buffer;
}

static bool setX509Certificate(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, char *deviceId)
{
    char certName[256];
    char keyName[256];
    char cwd[1024];

    getcwd(cwd, sizeof(cwd));
    snprintf(certName, sizeof(certName), "%s/%s-cert.pem", cwd, deviceId);
    snprintf(keyName, sizeof(keyName), "%s/%s-key.pem", cwd, deviceId);

    char *x509certificate = readFile(certName);
    char *x509privatekey = readFile(keyName);

    if (x509certificate == NULL ||
        x509privatekey == NULL ||
        IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_X509_CERT, x509certificate) != IOTHUB_CLIENT_OK ||
        IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_X509_PRIVATE_KEY, x509privatekey) != IOTHUB_CLIENT_OK)
    {
        printf("[Device] ERROR: Failed to set options for x509.\n");
        return false;
    }

    free(x509certificate);
    free(x509privatekey);

    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("[Device] IoT Hub connection string should be passed as a parameter\r\n");
        return 1;
    }

    char device_id[257];
    char *device_id_src = get_device_id(argv[1]);

    if (device_id_src == NULL)
    {
        printf("[Device] ERROR: Cannot parse device id from IoT device connection string\n");
        return 1;
    }

    snprintf(device_id, sizeof(device_id), "%s", device_id_src);
    free(device_id_src);

    wiringPiSetup();
    pinMode(LED_PIN, OUTPUT);

    IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

    (void)printf("[Device] Starting the IoTHub client sample MQTT...\r\n");

    if (platform_init() != 0)
    {
        printf("[Device] Failed to initialize the platform.\r\n");
    }
    else
    {
        if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(argv[1], MQTT_Protocol)) == NULL)
        {
            (void)printf("[Device] ERROR: iotHubClientHandle is NULL!\r\n");
        }
        else
        {
            if (strstr(argv[1], "x509=true") != NULL)
            {
                // Use X.509 certificate authentication.
                if (!setX509Certificate(iotHubClientHandle, device_id))
                {
                    return 1;
                }
            }

            IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL);

            while (!lastMessageReceived)
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
