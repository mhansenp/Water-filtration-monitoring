#ifndef LCD_CONTROLLER_H
#define LCD_CONTROLLER_H


#include <Wire.h>
#include <LiquidCrystal_I2C.h>


void LCD_setup(void);

void clear_Display();

void printMainFlow(void);

void printMainTemperature(void);

void printMainTDS(void);

void printMain(void);

void mainCursorPosition(int old_Pos, int new_Pos);

void flow_Menu();

void flow_Alarm_Update();

void TDS_menu();

void TDS_Alarm_Update();

void TDS_Calibration_Menu();

void update_Calibration();

void Temperature_Menu();

void Temperature_Alarm_Update();

void print_Save_TDS_Calibration();

#endif