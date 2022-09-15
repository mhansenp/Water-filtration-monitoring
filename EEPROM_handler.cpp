#include <EEPROM.h>
#include "EEPROM_handler.h"



//Define EEPROM addresses for each variable to be stored. 
//Assigning 8 bytes of storage to each value. 
int TDS_1_kvalue_address = 8;
int TDS_2_kvalue_address = 16;
int tds_min_reduction_alarm_address = 24;
int tds_max_pollutants_alarm_address = 32;
int flow_alarm_address = 40;
int min_temp_alarm_address = 48;


int get_TDS_1_kvalue_address(){return TDS_1_kvalue_address;}

int get_TDS_2_kvalue_address(){return TDS_2_kvalue_address;}

int get_tds_min_reduction_alarm_address(){return tds_min_reduction_alarm_address;}

int get_tds_max_pollutants_alarm_address(){return tds_max_pollutants_alarm_address;}

int get_flow_alarm_address(){return flow_alarm_address;}

int get_min_temp_alarm_address(){return min_temp_alarm_address;}


//void set_EEPROM(int address, float val){
//    EEPROM_write(address, val);
//}