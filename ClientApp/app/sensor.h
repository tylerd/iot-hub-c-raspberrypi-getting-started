#define PIN 7
#define TIMEOUT 255
#define RETRY_COUNT 10

typedef struct SENSOR_DATA_TAG
{
    float temperature;
    float humidity;
}Sensor_Data;

Sensor_Data * read_sensor_data();
Sensor_Data * read_sensor_data_with_retry();