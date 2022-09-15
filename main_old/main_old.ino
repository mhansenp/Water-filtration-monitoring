#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <stdio.h>
#include <stdlib.h>

#include <EEPROM.h>       // TDS readings
#include "GravityTDS.h"   //

//Defines analog inputs for TDC sensors
#define TdsSensorPinIn A6 
#define TdsSensorPinOut A7

#define ONE_WIRE_BUS 11 //Temperature Reading pin
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

//LCD Screen Pin
//int sensorPin = A3;    // select the input pin for the potentiometer
//int sensorValue = 0;  // variable to store the value coming from the sensor
char str_temperature[5];

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
float tempC;


void LCDsetup(void){
    lcd.begin(20,4); 
    lcd.setCursor(4,0);
    lcd.print("Water Quality");
    lcd.setCursor(0,1);
    lcd.print("Flow XXX/XXX = YY.Y%");
    lcd.setCursor(0,2);
    lcd.print("TDS  XXX/XXX = YY.Y%");
    lcd.setCursor(0,3);
    lcd.print("Temp XX.X C");

}

void TemperatureSetup(void){
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();
}

void TemperatureLoop(void){
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  tempC = sensors.getTempCByIndex(0);
  dtostrf(tempC, 4, 1, str_temperature);

  // Check if reading was successful
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    //Serial.print("Temperature for the device 1 (index 0) is: ");
    //Serial.println(tempC);
    lcd.setCursor(5,4);
    lcd.print(str_temperature);
  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
  }

}

//-------------------------------------------------------------------------------------- 
//    FLOW READINGS

volatile float  flow_frequency_clean;  // Measures flow meter pulses in
float  l_minute_clean;          // Calculated litres/hour                      
unsigned char flowmeter_in = 2;  // Flow Meter Pin number

volatile float  flow_frequency_waste;  // Measures flow meter pulses in
float  l_minute_waste;          // Calculated litres/hour                      
unsigned char flowmeter_out = 3;  // Flow Meter Pin number


float flowrate[2]; //Holds flowrates, clean at [0], waste at [1]
char str_percent_flow[5];


unsigned long currentTime;
unsigned long cloopTime;

// Interrupt functions for water flow
void flow_clean(){flow_frequency_clean++;} 
void flow_waste() {flow_frequency_waste++;} 


void flowSetup(){
   pinMode(flowmeter_in, INPUT); 
   pinMode(flowmeter_out, INPUT);
   Serial.begin(9600); 
   attachInterrupt(0, flow_clean, RISING); // Setup Interrupt 
   attachInterrupt(1, flow_waste, RISING);// see http://arduino.cc/en/Reference/attachInterrupt
   sei();                               // Enable interrupts  
   currentTime = millis();
   cloopTime = currentTime;
  
}

void getFlow(){
   currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {     
      cloopTime = currentTime;              // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min. (Results in +/- 3% range) ------- 23Q correction
      
      l_minute_clean = (flow_frequency_clean / 23.0); // (Pulse frequency x 60 min) / 7.5Q = flow rate in L/hour  ------- Not multiplying by 60. Keep in Liters per minute      
      //Serial.print(l_minute_clean, DEC);   //Debug
      //Serial.println(" L/minute clean");
      flow_frequency_clean = 0;                   // Reset Counter

      l_minute_waste = (flow_frequency_waste / 23); //Do same for outflow
      //Serial.print(l_minute_waste, DEC); //Debug
      //Serial.println(" L/minute waste");
      flow_frequency_waste = 0;

      flowrate[0] = l_minute_clean; 
      flowrate[1] = l_minute_waste; //Stores flowrates in and out.

   }
}

void Percentage_Flow(float flow_clean, float flow_waste){

    if(flow_clean + flow_waste == 0){
        str_percent_flow[0] = '0';
        str_percent_flow[1] = '0';
        str_percent_flow[2] = '.';
        str_percent_flow[3] = '0';   
    }else{
        float percentage_flow = (flow_clean / (flow_clean + flow_waste)) * 100;
        dtostrf(percentage_flow, 4, 1, str_percent_flow);
    }
  
}

void printFlow(){
  getFlow(); //Calls for a function to update our flow rates
  Percentage_Flow(flowrate[0], flowrate[1]);

  char in_flow[10];
  char out_flow[10];
  
  char str_rate_in [4]; //Buffer of length 4 allows for x.x/null but not values >=10
  char str_rate_out [4];
  
  dtostrf(flowrate[0], 3, 1, str_rate_in);
  dtostrf(flowrate[1], 3, 1, str_rate_out);
  
  lcd.setCursor(5, 1);
  lcd.print(str_rate_in);
  lcd.setCursor(9,1);
  lcd.print(str_rate_out);

  lcd.setCursor(15, 1);
  lcd.print(str_percent_flow);

  //Overrides overflow digits in case reading goes goes >=10 for either flow value
  lcd.setCursor(8, 1);
  lcd.print("/");
  lcd.setCursor(12, 1);
  lcd.print(" ");

  
}

//--------------------------------------------------------------------------------------
//      TDS READINGS

GravityTDS TdsIn;
GravityTDS TdsOut;

float temperature = 25,tdsValueIn = 0, tdsValueOut = 0;

void TDSsetup()
{
    Serial.begin(9600);
    //Initialize TDS reader for water input
    TdsIn.setPin(TdsSensorPinIn);
    TdsIn.setAref(3.3);  //reference voltage on ADC, default 5.0V on Arduino UNO
    TdsIn.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    TdsIn.begin();  //initialization

    //Initialize TDS reader for water output
    TdsOut.setPin(TdsSensorPinOut);
    TdsOut.setAref(3.3);  //reference voltage on ADC, default 5.0V on Arduino UNO
    TdsOut.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    TdsOut.begin();  //initialization
}

void TDSloop()
{
    temperature = tempC;  //Bad Practice lmao
    TdsIn.setTemperature(temperature);  // set the temperature and execute temperature compensation
    TdsIn.update();  //sample and calculate 
    tdsValueIn = TdsIn.getTdsValue();  // then get the value
    //Serial.print(tdsValueIn);
    //Serial.println("ppm");
    

    TdsOut.setTemperature(temperature);  // set the temperature and execute temperature compensation
    TdsOut.update();  //sample and calculate 
    tdsValueOut = TdsOut.getTdsValue();  // then get the value
    
    printTDS(tdsValueIn, tdsValueOut);
    delay(1000);
}

void printTDS(float TDS_in, float TDS_out){
    char str_TDS_in[4], str_TDS_out[4];
    dtostrf(TDS_in, 3, 0, str_TDS_in);
    dtostrf(TDS_out, 3, 0, str_TDS_out);

    lcd.setCursor(5, 2);
    lcd.print(str_TDS_in);
    lcd.setCursor(9, 2);
    lcd.print(str_TDS_out); 
} 

//----------------------------------------------------------------------------------------

void setup(void){
    LCDsetup();
    TemperatureSetup();
    flowSetup();
    TDSsetup();
}

void loop(void){
    TemperatureLoop();
    printFlow();
    //LCDloop();
    TDSloop();
}
