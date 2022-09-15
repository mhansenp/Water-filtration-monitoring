#include "State_Manager.h"
#include "LCD_Controller.h"
#include "Flow_Sensors.h"
#include "TDS_Sensors.h"
#include "EEPROM_handler.h"

enum displayState {MAIN, FLOW, TDS, TEMPERATURE, CALIBRATION};
displayState state = MAIN;
int selector_Location = 1;
int prev_Selector_Location;
bool editing_Var = false;
float temp;

void beep()
{
    tone(5, 880, 75); //tone(pin number, pitch, duration);
    delay(100); 
    noTone(5);
}

bool get_editing_state(){
    return editing_Var;
}

void update_Display()
{
    switch (state)
    {
        case MAIN:
            printMain();
            break;
        case FLOW:
            flow_Menu();
            break;
        case TDS:
            TDS_menu();
            break;
        case TEMPERATURE:
            Temperature_Menu();
            break;
        case CALIBRATION:
            TDS_Calibration_Menu();
            break;
    }
}

void action_Left()
{

    switch(state)
    {
        case MAIN: //At main screen
            break;

        case FLOW: //At flow screen
            beep();
            if(editing_Var == true)
            {
                editing_Var = false;
                mainCursorPosition(selector_Location, selector_Location);
            }
            else
            {
                state = MAIN;
                clear_Display();
                
                float new_Val = get_Flow_Alarm();
                float old_Val = read_flow_alarm_value();
                if(new_Val != old_Val) //Checks if the new flow value is different from the one stored in memory
                {
                    save_flow_alarm(new_Val);
                    Serial.println("Saving value to EEPROM");
                }else{
                    Serial.println("Did not save any new value to EEPROM");
                }
                
                mainCursorPosition(prev_Selector_Location, 1);
                prev_Selector_Location = 1;
                selector_Location = 1;
                printMain();
            }
            break;

        case TDS: //At TDS screen
            beep();
            if(editing_Var == true)
            {
                editing_Var = false;
                mainCursorPosition(selector_Location, selector_Location);
            }
            else
            {
                state = MAIN;
                clear_Display();

                int new_max_pollutants = get_max_pollutants();
                int old_max_pollutants =  read_max_pollutants();
                if(new_max_pollutants != old_max_pollutants)
                {
                    save_max_pollutants(new_max_pollutants);
                    Serial.println("Saving value to EEPROM");
                }else
                {
                    Serial.println("Did not save any new value to EEPROM");
                }

                float new_min_reduction = get_min_reduction();
                float old_min_reduction = read_min_reduction();
                if(new_min_reduction != old_min_reduction)
                {
                    save_min_reduction(new_min_reduction);
                    Serial.println("Saving value to EEPROM");
                }else
                {
                    Serial.println("Did not save any new value to EEPROM");
                }

                mainCursorPosition(prev_Selector_Location, 1);
                prev_Selector_Location = 1;
                selector_Location = 1;
                printMain();
            }
            break;

        case TEMPERATURE: //At Temperature Screen
            beep();
            if(editing_Var == true)
            {
                editing_Var = false;
                mainCursorPosition(selector_Location, selector_Location);
            }
            else
            {
                state = MAIN;
                clear_Display();

                float new_min_temp = get_min_temperature();
                float old_min_temp = read_min_temp();
                if(new_min_temp != old_min_temp)
                {
                    save_min_temp(new_min_temp);
                    Serial.println("Saving value to EEPROM");
                }else
                {
                    Serial.println("Did not save any new value to EEPROM");
                }


                mainCursorPosition(prev_Selector_Location, 1);
                prev_Selector_Location = 1;
                selector_Location = 1;
                printMain();
            }
            break;

        case CALIBRATION:
        beep();
            if(editing_Var == true)
            {
                editing_Var = false;
                mainCursorPosition(selector_Location, selector_Location);
            }
            else
            {
                clear_Display();
                state = TDS;
                mainCursorPosition(prev_Selector_Location, 1);
                selector_Location = 1; 
                prev_Selector_Location = selector_Location;
                update_Display();                  
                break;
            }

            break;
    }
    delay(400);
}

void action_Right()
{
    switch(state)
    {
        case MAIN: //At main screen
            beep();
            switch(selector_Location)
            {
                case 1: //Cursor on Flow
                    clear_Display();
                    state = FLOW;
                    mainCursorPosition(prev_Selector_Location, 1);
                    selector_Location = 1; 
                    prev_Selector_Location = selector_Location;
                    update_Display();
                    break;
                case 2: //Cursor on TDS
                    clear_Display();
                    state = TDS;
                    mainCursorPosition(prev_Selector_Location, 1);
                    selector_Location = 1; 
                    prev_Selector_Location = selector_Location;
                    update_Display();                  
                    break;
                case 3: //Cursor on Temp
                    clear_Display();
                    state = TEMPERATURE;
                    mainCursorPosition(prev_Selector_Location, 1);
                    selector_Location = 1;
                    prev_Selector_Location = selector_Location;
                    update_Display();
                    break;
            }
            delay(250); //Delay to avoid double presses when entering submenu from main menu
            break;

        case FLOW: //At flow screen
            if (editing_Var == false) 
            {
                beep();
                editing_Var = true;
                mainCursorPosition(selector_Location, selector_Location);     
            }
            break;

        case TDS: //At TDS screen
            if ((editing_Var == false) && (selector_Location != 3)) 
            {
                beep();
                editing_Var = true;
                mainCursorPosition(selector_Location, selector_Location);
                break;
            }else if(selector_Location == 3)
            {
                beep();
                clear_Display();
                state = CALIBRATION;
                mainCursorPosition(prev_Selector_Location, 1);
                selector_Location = 1; 
                prev_Selector_Location = selector_Location;
                update_Display();                  
                break;
            }
            break;
        case TEMPERATURE: //At Temperature Screen
            if (editing_Var == false) 
            {
                beep();
                editing_Var = true;
                mainCursorPosition(selector_Location, selector_Location);           
            }
            break;

        case CALIBRATION:
            if (selector_Location == 3)
            {
                beep();
                clear_Display();
                print_Save_TDS_Calibration();
                calibrate_sensors();
                state = MAIN;
                mainCursorPosition(prev_Selector_Location, 1);
                selector_Location = 1; 
                prev_Selector_Location = selector_Location;
                update_Display();                  
                break;
            }
            else if (editing_Var == false) 
            {
                beep();
                editing_Var = true;
                mainCursorPosition(selector_Location, selector_Location);           
            }
            break;
    }

}

void action_Top()
{
    switch(state){
        case MAIN:
            if (selector_Location != 1)
            {
                beep();
                prev_Selector_Location = selector_Location;
                selector_Location -= 1;
                mainCursorPosition(prev_Selector_Location, selector_Location);
            }
            else
            {
                Serial.println("Can't Move cursor up; Already at highest position");
            }
            break;
        
        case FLOW:
            if(editing_Var == true){
                beep();
                set_Flow_Alarm(2.5);
                delay(200);
            }
            break;
        case TDS:
            if(editing_Var == true){
                beep();
                switch(selector_Location){
                    case 1:
                        set_max_pollutants(1);
                        break;
                    case 2:
                        set_min_reduction(0.5);
                        break;
                }
            }else if (selector_Location != 1)
            {
                beep();
                prev_Selector_Location = selector_Location;
                selector_Location -= 1;
                mainCursorPosition(prev_Selector_Location, selector_Location);
                delay(500);
            }
            else
            {
                Serial.println("Can't Move cursor up; Already at highest position");
            }
            delay(200);
            break;
        case TEMPERATURE:
            if(editing_Var == true){
                beep();
                set_min_temperature(0.5);
                delay(200);
            }
            else
            {
                Serial.println("Can't Move cursor up; Already at highest position");
            }
            break;    

        case CALIBRATION:
            if(editing_Var == true){
                beep();
                switch(selector_Location){
                    case 1:
                        set_calibration_1(1);
                        break;
                    case 2:
                        set_calibration_2(1);
                        break;
                }
            }else if (selector_Location != 1)
            {
                beep();
                prev_Selector_Location = selector_Location;
                selector_Location -= 1;
                mainCursorPosition(prev_Selector_Location, selector_Location);
                delay(300);
            }
            else
            {
                Serial.println("Can't Move cursor up; Already at highest position");
            }
            delay(75);
            break;
    }
}

void action_Bottom()
{
    switch(state){
        case MAIN:
            if (selector_Location != 3)
            {
                beep();
                prev_Selector_Location = selector_Location;
                selector_Location += 1;
                mainCursorPosition(prev_Selector_Location, selector_Location);
            }
            else
            {
                Serial.println("Can't Move cursor down; Already at lowest position");
            }        
            break;

        case FLOW:
            if(editing_Var == true){
                beep();
                set_Flow_Alarm(-2.5);
                delay(200);
            }
            break;

        case TDS:
            if(editing_Var == true){
                beep();
                switch(selector_Location){
                    case 1:
                        set_max_pollutants(-1);
                        break;
                    case 2:
                        set_min_reduction(-0.5);
                        break;
                }
            }else if (selector_Location != 3)
            {
                beep();
                prev_Selector_Location = selector_Location;
                selector_Location += 1;
                mainCursorPosition(prev_Selector_Location, selector_Location);
                delay(500);
            }
            else
            {
                Serial.println("Can't Move cursor down; Already at lowest position");
            }        
            delay(200);
            break;

        case TEMPERATURE:
            if(editing_Var == true){
                beep();
                set_min_temperature(-0.5);
                delay(200);
            }
            else
            {
                Serial.println("Can't Move cursor down; Already at lowest position");
            }
            break;
        
        case CALIBRATION:
            if(editing_Var == true){
                beep();
                switch(selector_Location){
                    case 1:
                        set_calibration_1(-1);
                        break;
                    case 2:
                        set_calibration_2(-1);
                        break;
                }
            }else if (selector_Location != 3)
            {
                beep();
                prev_Selector_Location = selector_Location;
                selector_Location += 1;
                mainCursorPosition(prev_Selector_Location, selector_Location);
                delay(300);
            }
            else
            {
                Serial.println("Can't Move cursor down; Already at lowest position");
            }        
            delay(75);
            break;
    }
}
