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
#include "sensor.h"

#define SPI_CHANNEL 0
#define SPI_CLOCK 1000000L

#define DEFAULT_PERIOD 2000
#define BUFFER_SIZE 256
bool messagePending = false;

static void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    if (IOTHUB_CLIENT_CONFIRMATION_OK != result)
    {
        printf("[Device] Failed to send message to Azure IoT Hub\r\n");
    }

    messagePending = false;
}

static void sendMessages(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, char * buffer)
{
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, strlen(buffer));
    if (messageHandle == NULL)
    {
        printf("[Device] ERROR: unable to create a new IoTHubMessage\r\n");
    }
    else
    {
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            printf("[Device] ERROR: Failed to hand over the message to IoTHubClient\r\n");
        }
        else
        {
            messagePending = true;
            printf("[Device] Sending message: %s\r\n", buffer);
        }

        IoTHubMessage_Destroy(messageHandle);
    }
}

int createMessage(int messageId, char * buffer)
{
    snprintf(buffer, BUFFER_SIZE, "{ messageId: %d, temperature: %f, humidity: %f }", messageId, 10.0f, 20.0f);
    printf("%s\r\n", buffer);
}

static unsigned int BMEInitMark = 0;
#define BME_INIT 4
#define SPI_SETUP 2
#define WIRINGPI_SETUP 1
int readMessage(int messageId, char * buffer)
{
    // setup has not finished and try to init the BME280
    if (BMEInitMark != 7 && init_bme() != 1)
    {
        // setup failed
        return 0;
    }

    float temperature, humidity, pressure;
    if (bme280_read_sensors(&temperature, &pressure, &humidity) != 1)
    {
        snprintf(buffer, BUFFER_SIZE, "{ messageId: %d, error: \"read data failed\" }", messageId);
        return 0;
    }
    else
    {
        snprintf(buffer, BUFFER_SIZE, "{ messageId: %d, temperature: %f, humidity: %f }", messageId, temperature, humidity);
    }
    return 1;
}

int mask_check(int check, int mask)
{
    return (check & mask) ^ mask;
}

int init_bme()
{
    if (mask_check(BMEInitMark, WIRINGPI_SETUP) != 0 && wiringPiSetup() != 0)
    {
        return 0;
    }
    BMEInitMark |= WIRINGPI_SETUP;
    if (mask_check(BMEInitMark, SPI_SETUP) != 0 && wiringPiSPISetup(SPI_CHANNEL, SPI_CLOCK) < 0)
    {
        return 0;
    }
    BMEInitMark |= SPI_SETUP;
    if (mask_check(BMEInitMark, BME_INIT) != 0 && bme280_init(SPI_CHANNEL) != 1)
    {
        return 0;
    }
    BMEInitMark |= BME_INIT;
    return 1;
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
    // get period and get simulate data value
    int period = DEFAULT_PERIOD;
    bool simulateData = false;
    if (argc > 2)
    {
       period = atoi(argv[2]);
    }

    if (argc > 3 && !strcmp(argv[3], "true"))
    {
        printf("Use simulate data\r\n");
        simulateData = true;
    }
    else
    {
        printf("Use real device\r\n");
    }

    if (device_id_src == NULL)
    {
        printf("[Device] ERROR: Cannot parse device id from IoT device connection string\n");
        return 1;
    }

    snprintf(device_id, sizeof(device_id), "%s", device_id_src);
    free(device_id_src);

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

            int count = 0;
            while (true)
            {
                if (!messagePending)
                {
                    ++count;
                    char * buffer = (char *)malloc(BUFFER_SIZE);
                    if (buffer != NULL)
                    {
                        int readMessageResult = simulateData ? createMessage(count, buffer) : readMessage(count, buffer);
                        if (readMessageResult == 1)
                        {
                            sendMessages(iotHubClientHandle, buffer);
                        }
                        free(buffer);
                    }
                }
                IoTHubClient_LL_DoWork(iotHubClientHandle);
                delay(period);
            }

            IoTHubClient_LL_Destroy(iotHubClientHandle);
        }
        platform_deinit();
    }

    return 0;
}
