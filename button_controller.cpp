#include "button_controller.h"
#include <OneWire.h>
//#include "LCD_Controller.h"
#include "State_Manager.h"
#include "EEPROM_handler.h"


//Buttons:  D0 D1 D7 D8 


int state_Left = 0;
int state_Right = 0;
int state_Top = 0;
int state_Bottom = 0;

char incoming_char;

/*
int prev_State_Left;
int prev_State_Right;
int prev_State_Top;
int prev_State_Bottom;
*/  

//Declare pin locations
const int button_Left   = 6;
const int button_Right  = 7;
const int button_Top    = 8;
const int button_Bottom = 9;  

void button_Setup(){
    //Set pinmodes as inputs for pins specified in above declaration
    pinMode(button_Left, INPUT);
    pinMode(button_Right, INPUT);
    pinMode(button_Top, INPUT);
    pinMode(button_Bottom, INPUT);

    //prev_State_Left     = 0;
    //prev_State_Right    = 0;
    //prev_State_Top      = 0;
    //prev_State_Bottom   = 0;
}

int read_Buttons(){
    state_Left = digitalRead(button_Left);
    state_Right = digitalRead(button_Right);
    state_Top = digitalRead(button_Top);
    state_Bottom = digitalRead(button_Bottom);

    if (state_Bottom == HIGH)
    {
        Serial.println("Trigger Bottom");
        action_Bottom();
        state_Bottom = LOW;
    }else if(state_Left == HIGH){
        Serial.println("Trigger Left");
        action_Left();
        state_Left = LOW;
    }else if (state_Right == HIGH)
    {
        Serial.println("Trigger Right");
        action_Right();
        state_Right = LOW;
    }else if (state_Top == HIGH)
    {
        Serial.println("Trigger Top");
        action_Top();
        state_Top = LOW;
    }else
    {
        //Serial.println("No Trigger");
        read_serial();
        //printNone();
    }
}

void read_serial(){
    //Serial.println("Checking for serial input");
    if (Serial.available() > 0) {
        Serial.println("Serial input found");
        // read the incoming byte:
        incoming_char = Serial.read();
        switch(incoming_char)
        {
            case 's':
                Serial.println("Trigger Bottom");
                action_Bottom();
                break;

            case 'a':
                Serial.println("Trigger Left");
                action_Left();
                break;

            case 'd':
                Serial.println("Trigger Right");
                action_Right();
                break;

            case 'w':
                Serial.println("Trigger Top");
                action_Top();
                break;

            case 'r': //Read EEPROM to serial - Debugging
                action_Top();
                float temp;
                EEPROM_read(8, temp);
                Serial.print("EEPROM at address 8: ");
                Serial.println(temp);
                break;

            default:
                Serial.print("No valid input");
                break;

        }
  }
}