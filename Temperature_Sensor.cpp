#include "Temperature_Sensor.h"

#include <OneWire.h>
#include <DallasTemperature.h>
#include "EEPROM_handler.h"

#define MIN_TEMP_ADDRESS 48

    // Start up the library
    // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

float tempC = 0;
float min_temperature;

void temperatureSetup(void)
{
    // start serial port
    Serial.begin(9600); //Baud Rate? Comms protocol?
    sensors.begin();

    updateTemperature();

    EEPROM_read(MIN_TEMP_ADDRESS, min_temperature);
    if(!((min_temperature >= 0) && (min_temperature <= 50)))
    {
        min_temperature = 20;
        EEPROM_write(MIN_TEMP_ADDRESS, min_temperature);
    }
    
}

void updateTemperature(void)
{
    // call sensors.requestTemperatures() to issue a global temperature 
    // request to all devices on the bus
    //Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    //Serial.println("DONE");
    // After we got the temperatures, we can print them here.
    // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    tempC = sensors.getTempCByIndex(0);


    // Check if reading was successful
    if(tempC != DEVICE_DISCONNECTED_C) 
    {
        //Serial.print("Temperature for the device 1 (index 0) is: ");
        //Serial.println(tempC);
    } 
    else
    {
        Serial.println("Error: Could not read temperature data");
        tempC = 0;
    }

}

float getTemp()
{
  return tempC;
}

float get_min_temperature()
{
    return min_temperature;
}

void set_min_temperature(float value)
{
    if (((min_temperature + value) >= 0) && ((min_temperature + value) <= 40)){
        min_temperature += value;
    }
}

float read_min_temp()
{
    float temp;
    EEPROM_read(MIN_TEMP_ADDRESS, temp);
    return temp;
}

void save_min_temp(float val)
{
    EEPROM_write(MIN_TEMP_ADDRESS, val);
}