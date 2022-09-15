#ifndef EEPROM_HANDLER_H
#define EEPROM_HANDLER_H

#include <EEPROM.h>

#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
#define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}

/*
int get_TDS_1_kvalue_address();
int get_TDS_2_kvalue_address();
int get_tds_min_reduction_alarm_address();
int get_tds_max_pollutants_alarm_address();
int get_flow_alarm_address();
int get_min_temp_alarm_address();
*/

//void set_EEPROM(int address, float val);


#endif