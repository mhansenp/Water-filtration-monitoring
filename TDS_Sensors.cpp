#include "TDS_Sensors.h"
#include "EEPROM_handler.h"

#define TDS_KVALUE_ADDRESS_1 8
#define TDS_KVALUE_ADDRESS_2 16
#define TDS_MIN_REDUCTION_ADDRESS 24
#define TDS_MAX_POLLUTANTS_ADDRESS 32

GravityTDS tdsIn;
GravityTDS tdsOut;

float temperature = 25,tdsValueIn = 0, tdsValueOut = 0, min_Reduction = 0;
int calibration_1 = 0, calibration_2 = 0, max_pollutants = 100;

void tdsSetup()
{
    Serial.begin(9600);
    //Initialize TDS reader for tap water input
    tdsIn.setAref(3.3);  //reference voltage on ADC, default 5.0V on Arduino UNO
    tdsIn.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    tdsIn.setKvalueAddress(TDS_KVALUE_ADDRESS_1); //Sets EEPROM address to store K value for calibration
    tdsIn.setPin(TdsSensorPinIn);
    tdsIn.begin();  //initialization

    //Initialize TDS reader for clean water output
    tdsOut.setPin(TdsSensorPinOut);
    tdsOut.setAref(3.3);  //reference voltage on ADC, default 5.0V on Arduino UNO
    tdsOut.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    tdsOut.setKvalueAddress(TDS_KVALUE_ADDRESS_2); //Sets EEPROM address to store K value for calibration
    tdsOut.begin();  //initialization

    EEPROM_read(TDS_MIN_REDUCTION_ADDRESS, min_Reduction); //Sets minimum pollutant reduction % to saved value
    EEPROM_read(TDS_MAX_POLLUTANTS_ADDRESS, max_pollutants); //Sets Max pollutants in output to saved value

    if(!((min_Reduction >= 0) && (min_Reduction <= 100))) //Checks if the value from EEPROM is within acceptable bounds.
    {
       min_Reduction = 50; //Sets flow alarm to a default value
       EEPROM_write(TDS_MIN_REDUCTION_ADDRESS, min_Reduction);
    }
    
    if(!((max_pollutants >= 0) && (max_pollutants <= 500))) //Checks if the value from EEPROM is within acceptable bounds.
    {
       max_pollutants = 150; //Sets flow alarm to a default value
       EEPROM_write(TDS_MAX_POLLUTANTS_ADDRESS, max_pollutants);
    }
}

void updateTDS()
{
    //Serial.println("Getting TDS Readings from sensors");
    temperature = getTemp(); 
    tdsIn.setTemperature(temperature);  // set the temperature and execute temperature compensation
    tdsIn.update();  //sample and calculate 
    tdsValueIn = tdsIn.getTdsValue();  // then get the value

    tdsOut.setTemperature(temperature);  // set the temperature and execute temperature compensation
    tdsOut.update();  //sample and calculate 
    tdsValueOut = tdsOut.getTdsValue();  // then get the value
    //Serial.println("Received TDS Readings from sensors");
}

float percentage_Effectiveness(){
    if(tdsValueIn + tdsValueOut == 0){
        return 0;
    }else{
        float percent_effectiveness = (tdsValueOut / tdsValueIn) * 100;  
        return percent_effectiveness;
    }
}

float getTdsIn(){
    //Serial.println("Sending TDS values to LCD Controller");
    return tdsValueIn;
}

float getTdsOut(){
    return tdsValueOut;
}

float get_min_reduction()
{
    return min_Reduction;
}

int get_max_pollutants()
{
    return max_pollutants;
}

int get_calibration_1(){
    return calibration_1;
}

int get_calibration_2(){
    return calibration_2;
}

void set_min_reduction(float value){
    if( ((min_Reduction + value) >= 0) && ((min_Reduction + value) <= 100)){
        min_Reduction += value;
    }
}


void set_max_pollutants(float value){
    if( ((max_pollutants + value) >= 0) && ((max_pollutants + value) <= 500)){
        max_pollutants += value;
    }
}

void set_calibration_1(float value){
    if( ((calibration_1 + value) >= 0) && ((calibration_1 + value) <= 500)){
        calibration_1 += value;
    }
}

void set_calibration_2(float value){
    if( ((calibration_2 + value) >= 0) && ((calibration_2 + value) <= 500)){
        calibration_2 += value;
    }
}

void calibrate_sensors()
{
    tdsOut.sensor_Calibration(calibration_1);
    tdsIn.sensor_Calibration(calibration_2);
}

int read_max_pollutants()
{
    int temp;
    EEPROM_read(TDS_MAX_POLLUTANTS_ADDRESS, temp);
    return temp;
}

void save_max_pollutants(int val)
{
    EEPROM_write(TDS_MAX_POLLUTANTS_ADDRESS, val);
}

float read_min_reduction()
{
    float temp;
    EEPROM_read(TDS_MIN_REDUCTION_ADDRESS, temp);
    return temp;
}

void save_min_reduction(float val)
{
    EEPROM_write(TDS_MIN_REDUCTION_ADDRESS, val);
}
