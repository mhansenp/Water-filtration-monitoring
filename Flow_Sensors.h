#ifndef FLOW_SENSORS_H
#define FLOW_SENSORS_H

#include <OneWire.h>


    //Triggered by interrupt
    void flow_clean(); 
    void flow_input(); 

    //Sets pinmodes and interrupts.
    void flowSetup(); 

    //Updates values for clean and waste water flowing through.
    void getFlow();

    //Calculates percentage of water flow is clean vs total water flowing through the system.
    float Percentage_Flow(float flow_clean, float flow_input);

    float get_Clean(void);
    float get_Input(void);

    float get_Flow_Alarm();

    void set_Flow_Alarm(float value);

    void save_flow_alarm(float val);
    float read_flow_alarm_value();

#endif