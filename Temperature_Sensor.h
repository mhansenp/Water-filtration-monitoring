#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4 //Temperature Reading pin



//Setup sensor. Virtually pointless aside from sensors.begin() function call
void temperatureSetup(void);

void updateTemperature(void);

float getTemp();

float get_min_temperature();

void set_min_temperature(float value);

float read_min_temp();

void save_min_temp(float val);

#endif