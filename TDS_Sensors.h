#ifndef TDS_SENSORS_H
#define TDS_SENSORS_H

#include <EEPROM.h>       // TDS readings
#include "GravityTDSmod.h"   //
#include "Temperature_Sensor.h"

//Defines analog inputs for TDC sensors
#define TdsSensorPinIn A6 
#define TdsSensorPinOut A7

//Initiates TDS Readers; Sets pinmode, analog range (10 bit), and reference voltage.
void tdsSetup();

//Gets reading from sensors and updates global variables
void updateTDS();

//Calculates effectiveness of the system by calculating the percentage of particles removed by filters. 
float percentage_Effectiveness();

//Returns TDS values of water flowing into the system (dirty tap water)
float getTdsIn();
//Returns TDS values of water flowing out of the system (clean filtered water)
float getTdsOut();

float get_min_reduction();
int get_max_pollutants();

int get_calibration_1();
int get_calibration_2();

void set_min_reduction(float value);
void set_max_pollutants(float value);

void set_calibration_1(float value);
void set_calibration_2(float value);

void calibrate_sensors();

int read_max_pollutants();
void save_max_pollutants(int val);

float read_min_reduction();
void save_min_reduction(float val);

#endif