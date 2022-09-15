#include "Flow_Sensors.h"
#include "EEPROM_handler.h"

#define FLOW_ALARM_ADDRESS 40

volatile float  flow_frequency_clean;  // Measures flow meter pulses in
float  l_minute_clean;          // Calculated litres/hour                      
unsigned char flowmeter_clean = 2;  // Flow Meter Pin number

volatile float  flow_frequency_input;  // Measures flow meter pulses in
float  l_minute_input;          // Calculated litres/hour                      
unsigned char flowmeter_input = 3;  // Flow Meter Pin number


float flowrate[2]; //Holds flowrates, clean at [0], input at [1]
char str_percent_flow[5];

float flow_Alarm; //Value of the minimum ratio (percentage) of clean to waste water flow in the system

unsigned long currentTime;
unsigned long cloopTime;

// Interrupt functions for water flow 
void flow_clean(){flow_frequency_clean++;} 
void flow_input() {flow_frequency_input++;} 


void flowSetup(){
   pinMode(flowmeter_clean, INPUT); 
   pinMode(flowmeter_input, INPUT);
   Serial.begin(9600); 
   attachInterrupt(0, flow_clean, RISING); // Setup Interrupt 
   attachInterrupt(1, flow_input, RISING);// see http://arduino.cc/en/Reference/attachInterrupt
   sei();                               // Enable interrupts  
   currentTime = millis();
   cloopTime = currentTime;

   EEPROM_read(FLOW_ALARM_ADDRESS, flow_Alarm);
   if(!((flow_Alarm >= 0) && (flow_Alarm <= 100))) //Checks if the value from EEPROM is within acceptable bounds.
   {
       flow_Alarm = 50; //Sets flow alarm to a default value
       EEPROM_write(FLOW_ALARM_ADDRESS, flow_Alarm);
   }
   
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

      l_minute_input = (flow_frequency_input / 23); //Do same for outflow
      //Serial.print(l_minute_waste, DEC); //Debug
      //Serial.println(" L/minute waste");
      flow_frequency_input = 0;

      flowrate[0] = l_minute_clean; 
      flowrate[1] = l_minute_input; //Stores flowrates in and out.

//      Serial.print("Clean Flow: ");
//      Serial.println(flowrate[0]);
//      Serial.print("Input Flow: ");
//      Serial.println(flowrate[1]);


   }
}

float Percentage_Flow(float flow_clean, float flow_input){
    if(flow_clean + flow_input == 0){
       return 0; 
    }else{
        float percentage_flow = (flow_clean / flow_input) * 100;
        return percentage_flow;
    }
  
}

float get_Clean(void)
{
    return flowrate[0];
}
float get_Input(void)
{
    return flowrate[1];
}

float get_Flow_Alarm()
{
return flow_Alarm;
}

//Change flow alarm by a certain value
void set_Flow_Alarm(float value)
{
    flow_Alarm += value;
}

//Saves new alarm value to EEPROM.
void save_flow_alarm(float val)
{
    EEPROM_write(FLOW_ALARM_ADDRESS, val);
}

//Reads value from the EEPROM rather than DRAM. Used on startup.
float read_flow_alarm_value()
{
    float val;
    EEPROM_read(FLOW_ALARM_ADDRESS, val)
    return val;
}