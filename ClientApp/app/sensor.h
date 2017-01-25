typedef struct SENSOR_DATA_TAG
{
    float temperature;
    float humidity;
}Sensor_Data;

Sensor_Data * read_sensor_data(); 