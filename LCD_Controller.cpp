#include "LCD_Controller.h"
#include "TDS_Sensors.h"
#include "Temperature_Sensor.h"
#include "Flow_Sensors.h"
#include "State_Manager.h"

//LCD Screen Pin ???????????????????????????????????
//int sensorPin = A3;    // select the input pin for the potentiometer            ???????????????????????
//int sensorValue = 0;  // variable to store the value coming from the sensor     ???????????????????????
/* ?????????????????????????????????? */

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address


void LCD_setup(void)
{
    lcd.begin(20,4);  
    lcd.setCursor(4,0);
    lcd.print("Water Quality");
    lcd.setCursor(1,1);
    lcd.print("Flow    /    =    %");
    lcd.setCursor(1,2);
    lcd.print("TDS     /    =    %");
    lcd.setCursor(1,3);
    lcd.print("Temp      C");
    mainCursorPosition(1, 1); //Draws cursor at start position.
}

void clear_Display()
{
    lcd.clear();
}

void printMainFlow()
{
    getFlow(); //Calls for a function to update our flow rates
    
    float flow_Clean = get_Clean();
    float flow_Input = get_Input();
    float percent_flow = Percentage_Flow(flow_Clean, flow_Input);
    
    char str_rate_Clean[10]; //Extra characters available in array for redundancy.
    char str_rate_Input[10];
    char str_percent_flow[10];
    
    dtostrf(flow_Clean, 3, 1, str_rate_Clean);
    dtostrf(flow_Input, 3, 1, str_rate_Input);
    dtostrf(percent_flow, 4, 1, str_percent_flow);
    
    lcd.setCursor(6, 1);
    lcd.print(str_rate_Clean);
    lcd.setCursor(10,1);
    lcd.print(str_rate_Input);

    lcd.setCursor(15, 1);
    lcd.print(str_percent_flow);

    //Overrides overflow digits in case reading goes goes >=10 for either flow value
    lcd.setCursor(9, 1);
    lcd.print("/");
    lcd.setCursor(13, 1);
    lcd.print(" ");
    lcd.setCursor(19, 1);
    lcd.print("%");
 
}

void printMainTemperature()
{
    //updateTemperature();
    float temperature = getTemp();

    //Char array stores temperature as a formatted string ready to write to display
    char str_temperature[10]; //Extra characters available in array for redundancy.
    dtostrf(temperature, 4, 1, str_temperature); //Store float as formatted character array
    
    //Set cursor and print temperature string
    lcd.setCursor(6,3);
    lcd.print(str_temperature);
}

void printMainTDS()
{
    updateTDS();
    char str_TDS_in[10], str_TDS_out[10]; //Extra characters available in array for redundancy.
    char str_TDS_effect[10];              //Extra characters available in array for redundancy.
    dtostrf(getTdsIn(), 3, 0, str_TDS_in);
    dtostrf(getTdsOut(), 3, 0, str_TDS_out);
    dtostrf(percentage_Effectiveness(), 3, 0, str_TDS_effect);

    //Prints clean water TDS level first
    lcd.setCursor(6, 2);
    lcd.print(str_TDS_out);
    
    //Prints unfiltered water's TDS level second
    lcd.setCursor(10, 2);
    lcd.print(str_TDS_in); 

    lcd.setCursor(15, 2);
    lcd.print(str_TDS_effect);

    //Overrides overflow digits in case reading goes goes >=100 for either TDS value
    lcd.setCursor(9, 2);
    lcd.print("/");
    lcd.setCursor(13, 2);
    lcd.print(" ");
    lcd.setCursor(19, 2);
    lcd.print("%");

}

void printMain()
{
    lcd.setCursor(4,0);
    lcd.print("Water Quality");
    lcd.setCursor(1,1);
    lcd.print("Flow");
    lcd.setCursor(14, 1);
    lcd.print("=");
    lcd.setCursor(1,2);
    lcd.print("TDS");
    lcd.setCursor(14, 2);
    lcd.print("=");
    lcd.setCursor(1,3);
    lcd.print("Temp");
    lcd.setCursor(11, 3);
    lcd.print("C");
    
    printMainTDS(); //TROUBLE MAKER
    printMainFlow();
    printMainTemperature();

} 

void mainCursorPosition(int old_Pos, int new_Pos)
{
    lcd.setCursor(0, old_Pos);
    lcd.print(" ");
    lcd.setCursor(0, new_Pos);
    if(get_editing_state() == false){
        lcd.write(126);
    }else{
        lcd.write(62);
    }
    
}

void flow_Menu()
{
    lcd.setCursor(0, 0);
    lcd.print("Flow Alarm");
    lcd.setCursor(1, 1);
    lcd.print("Production <    %");
    flow_Alarm_Update();
}

void flow_Alarm_Update(){
    char flow_alarm_str[5];
    dtostrf(get_Flow_Alarm(), 4, 1, flow_alarm_str);
    lcd.setCursor(13, 1);
    lcd.print(flow_alarm_str);
}

void TDS_menu(){
    lcd.setCursor(0, 0);
    lcd.print("TDS Alarm & Calibrat");
    
    lcd.setCursor(1, 1);
    lcd.print("Clean       >");
    lcd.setCursor(17, 1);
    lcd.print("ppm");
    
    lcd.setCursor(1, 2);
    lcd.print("Reduction    <");
    lcd.setCursor(19, 2);
    lcd.print("%");

    lcd.setCursor(1, 3);
    lcd.print("Calibrate");
    TDS_Alarm_Update();
}

void TDS_Alarm_Update(){
    char tds_reduction_str[5];
    dtostrf(get_min_reduction(), 4, 1, tds_reduction_str);

    lcd.setCursor(14, 1);
    lcd.print("   ");
    lcd.setCursor(14, 1);
    lcd.print(get_max_pollutants());

    lcd.setCursor(15, 2);
    lcd.print("   ");
    lcd.setCursor(15, 2);
    lcd.print(tds_reduction_str);    
}

void TDS_Calibration_Menu(){
    lcd.setCursor(0, 0);
    lcd.print("Set accepted values");
    lcd.setCursor(1, 1); 
    lcd.print("Sensor out: ");
    lcd.setCursor(17, 1);
    lcd.print("ppm");
    lcd.setCursor(1, 2);
    lcd.print("Sensor in: ");
    lcd.setCursor(17, 2);
    lcd.print("ppm");
    lcd.setCursor(1, 3);
    lcd.print("Save and Calibrate");
    update_Calibration();
}

void update_Calibration(){
    char tds_calibration1_str[5];
    char tds_calibration2_str[5];
    dtostrf(get_calibration_1(), 3, 0, tds_calibration1_str);
    dtostrf(get_calibration_2(), 3, 0, tds_calibration2_str);

    lcd.setCursor(13, 1);
    lcd.print(tds_calibration1_str);
    lcd.setCursor(13, 2);
    lcd.print(tds_calibration2_str);
}

void Temperature_Menu()
{
    lcd.setCursor(0, 0);
    lcd.print("Temperature Alarm");
    lcd.setCursor(1, 1);
    lcd.print("Temperature >");
    lcd.setCursor(18, 1);
    lcd.print("C");

    Temperature_Alarm_Update();
}

void Temperature_Alarm_Update()
{
    char min_temperature_str[5];
    dtostrf(get_min_temperature(), 4, 1, min_temperature_str);
    lcd.setCursor(14, 1);
    lcd.print(min_temperature_str);
}

void print_Save_TDS_Calibration()
{
    lcd.setCursor(4, 0);
    lcd.print("TDS Sensor"); //10 Chars
    lcd.setCursor(1, 1);
    lcd.print("Calibration Saved"); //17 Chars
    delay(1000);
    clear_Display();

}