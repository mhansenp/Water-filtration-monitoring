#include "LCD_Controller.h"
#include "TDS_Sensors.h"
#include "Temperature_Sensor.h"
#include "Flow_Sensors.h"
#include "button_controller.h"
#include "State_Manager.h"

int counter = 0;

void timer_setup(){
  cli();//stop interrupts
  
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
}



ISR(TIMER1_COMPA_vect){  //change the 0 to 1 for timer1 and 2 for timer2
   counter++;
   if(counter == 30){
    counter = 0;
    updateTemperature();
   }
}

void setup()
{
    button_Setup();
    LCD_setup();
    temperatureSetup();
    flowSetup();
    tdsSetup();
    timer_setup();
    delay(100);
}

void loop(void)
{   
    read_Buttons();
    update_Display();
}
