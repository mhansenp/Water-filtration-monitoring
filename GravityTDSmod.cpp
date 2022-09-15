/***************************************************
 DFRobot Gravity: Analog TDS Sensor/Meter
 <https://www.dfrobot.com/wiki/index.php/Gravity:_Analog_TDS_Sensor_/_Meter_For_Arduino_SKU:_SEN0244>
 
 ***************************************************
 This sample code shows how to read the tds value and calibrate it with the standard buffer solution.
 707ppm(1413us/cm)@25^c standard buffer solution is recommended.
 
 Created 2018-1-3
 By Jason <jason.ling@dfrobot.com@dfrobot.com>
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/

#include <EEPROM.h>
#include "EEPROM_handler.h"
#include "GravityTDSmod.h"


//#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
//#define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}

GravityTDS::GravityTDS() //Default Constructor
{
    this->pin = A1;
    this->temperature = 25.0;
    this->aref = 5.0;
    this->adcRange = 1024.0;
    this->kValueAddress = 8;
    this->kValue = 1.0;
}

GravityTDS::~GravityTDS() //Destructor
{
}

void GravityTDS::setPin(int pin) //Pin by which sensor is connected
{
	this->pin = pin;
}

void GravityTDS::setTemperature(float temp) //Current Temperature of water at sensor
{
	this->temperature = temp;
}

void GravityTDS::setAref(float value) //Analog Reference Voltage
{
	this->aref = value;
}

void GravityTDS::setAdcRange(float range) //Analog range of values, typically 0 to 1024
{
      this->adcRange = range;
}

void GravityTDS::setKvalueAddress(int address) //Address where K value is stored. Used in calibrating sensors
{
      this->kValueAddress = address;
}

void GravityTDS::begin() 
{
	pinMode(this->pin,INPUT);
	readKValues();
}

float GravityTDS::getKvalue()
{
	return this->kValue;
}

void GravityTDS::update()
{
	this->analogValue = analogRead(this->pin);
	this->voltage = this->analogValue/this->adcRange*this->aref;
	this->ecValue=(133.42*this->voltage*this->voltage*this->voltage - 255.86*this->voltage*this->voltage + 857.39*this->voltage)*this->kValue;
	this->ecValue25  =  this->ecValue / (1.0+0.02*(this->temperature-25.0));  //temperature compensation
	this->tdsValue = ecValue25 * TdsFactor;
	/*
  if(cmdSerialDataAvailable() > 0)
        {
            ecCalibration(cmdParse());  // if received serial cmd from the serial monitor, enter into the calibration mode
        }
  */
}

float GravityTDS::getTdsValue()
{
	return tdsValue;
}

float GravityTDS::getEcValue()
{
      return ecValue25;
}


void GravityTDS::readKValues()
{
    EEPROM_read(this->kValueAddress, this->kValue);  
    if(EEPROM.read(this->kValueAddress)==0xFF && EEPROM.read(this->kValueAddress+1)==0xFF && EEPROM.read(this->kValueAddress+2)==0xFF && EEPROM.read(this->kValueAddress+3)==0xFF)
    {
      this->kValue = 1.0;   // default value: K = 1.0
      EEPROM_write(this->kValueAddress, this->kValue);
    }
}

void GravityTDS::sensor_Calibration(int accepted_TDS)
{
    char *cmdReceivedBufferPtr;
    float KValueTemp,rawECsolution;
  
    rawECsolution = (float)(accepted_TDS)/(float)(TdsFactor);
    rawECsolution = rawECsolution*(1.0+0.02*(temperature-25.0));
  
          KValueTemp = rawECsolution/(133.42*voltage*voltage*voltage - 255.86*voltage*voltage + 857.39*voltage);  //calibrate in the  buffer solution, such as 707ppm(1413us/cm)@25^c
          if((rawECsolution>0) && (rawECsolution<2000) && (KValueTemp>0.25) && (KValueTemp<4.0))
          {
              Serial.println();
              Serial.print(F(">>>Confirm Successful, K:"));
              Serial.print(KValueTemp);
              kValue =  KValueTemp;
          }
          else{
            Serial.println(); 
            Serial.println(F(">>>Confirm Failed,Try Again<<<"));
            Serial.println();
          }        
        EEPROM_write(kValueAddress, kValue);
}
    
  
