#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Hydroponics_Cmds.h"
#include "Arduino.h"
#include "configs.h"
#include <EEPROM.h>


#define NODE_DATA_SIZE 4
#define STD_BUF_SIZE 20
#define STD_MOTOR_SPD 256/2

typedef enum state {
  LOW_PRESSURE,
  HIGH_PRESSURE,
  STANDARD,
  BOOT,
  OFF,
  END
};

extern char stateNameArr[][STD_BUF_SIZE];

typedef struct _eeLookup {
  char n[STD_BUF_SIZE];
  int addr;
};

extern int EE_LOC_size;
extern struct _eeLookup EE_LOC[];

#include "Serial_Handle.h"
#include "mlist.h" //has to go here because compiler seems to be linear


#endif
