#include "Hydroponics_Cmds.h"

int motor_speed = 0;
int pVal = 0;
int forceOverride = 0;

int cmd_arr_size = 14;
struct cmd_entry cmd_arr[] = {
  {"state", get_curr_state},
  {"change_state", force_state_change},
  {"force_on", force_on},
  {"force_off", force_off},
  {"reset", reset_func},
  {"print_buffer", printSerialBuffer},
  {"print_config", printConfig},
  {"eeSet", setEEPROMvar},
  {"eeGet", getEEPROMval},
  {"getSpeed", getSpeeda},
  {"getPressure", getPressure},
  {"print_eeVars", print_eeVarArr},
  {"extReset_val", print_extReset_val},
  {"help", help_func}
};

void print_extReset_val(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    Serial.println(analogRead(vConf.reset_ext));
  }
}

void print_eeVarArr(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    for(int i = 0; i < EE_LOC_size; i++) {
      Serial.print("->");
      Serial.println(EE_LOC[i].n);
    }
  }
}

int getPReading(struct STATE_MACH *m) {
  //TODO: update to pressure reading
  //need calibration function as well
  if(vConf.pAttached) {
    pVal = analogRead(vConf.iPSens);
  } else {
    //if pressure sensor is detached, randomly generate signal for testing purposes
    // pVal = random(0, 1024);
    struct STATE_NODE *h = m->head;

    int upper = 0, lower = 0;
    while(h != NULL) {
      if(m->curr == h->sName) {
        lower = h->d[0];
        upper = h->d[1];
        break;
      }
      h = h->next;
    }

    //add 10 on either side of the limits if available, this will reduce the amount of times that the state changes
    if((lower - 10) > 0) lower-= 1;
    if((upper + 10) < 1024) upper += 1;

    pVal = random(lower, upper);
  }
  return pVal;
}

void getPressure(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    itoa(getPReading(st), out, 10);
  }
}

void getEEPROMval(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    char buf[STD_BUF_SIZE];
    memset(buf, 0x0, STD_BUF_SIZE * sizeof(char));

    char argsBuf[STD_BUF_SIZE];
    memset(argsBuf, 0x0, STD_BUF_SIZE * sizeof(char));
    strcpy(argsBuf, args);

    int addr = -1;
    for(int i = 0; i < EE_LOC_size; i++) {
      if(strcmp(argsBuf, EE_LOC[i].n) == 0) {
        addr = EE_LOC[i].addr;
        break;
      }
    }

    if(addr == -1) addr = atoi(argsBuf);

    int ret = 0;

    EEPROM.get(addr, ret);

    itoa(ret, buf, 10);

    strcpy(out, buf);
  }
}

void setEEPROMvar(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    char argsBuf[STD_BUF_SIZE];
    memset(argsBuf, 0x0, STD_BUF_SIZE * sizeof(char));
    strcpy(argsBuf, args);

    char targetBuf[STD_BUF_SIZE];
    char valBuf[STD_BUF_SIZE];
    memset(targetBuf, 0x0, STD_BUF_SIZE * sizeof(char));
    memset(valBuf, 0x0, STD_BUF_SIZE * sizeof(char));

    split_by_space(argsBuf, targetBuf, valBuf);

    for(int i = 0; i < EE_LOC_size; i++) {
      if(strcmp(targetBuf, EE_LOC[i].n) == 0) {
        int val = atoi(valBuf);

        EEPROM.put(EE_LOC[i].addr, val);
        
        break;
      }
    }
  }
}

void printConfig(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    sprintf(out, "M1: %d / M2: %d / PSens: %d / PLow: %d / PHigh: %d / PAttach: %d / RESETL: %d / RESETH: %d / RESETEXT: %d", \
      vConf.oMotor1, vConf.oMotor2, vConf.iPSens, vConf.pSenseLow, vConf.pSenseHigh, vConf.pAttached, vConf.reset_low, vConf.reset_high, vConf.reset_ext);
  }
}

void(* resetFunc) (void) = 0;

void reset_func(struct STATE_MACH *st, void *args, int argc, char *out) {
  Serial.println("Resetting...");
  delay(1000);
  resetFunc();
}

void help_func(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    for(int i = 0; i < cmd_arr_size; i++) {
      Serial.print("-> ");
      Serial.println(cmd_arr[i].cmd);
    } 
    Serial.println();
    out = NULL;
  }
}

void getSpeeda(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    itoa(motor_speed, out, 10);
  }
}

int getSpeedi() {
  return motor_speed;
}

void set_motor_speed(int duty) {
  if(abs(duty) < 0 || abs(duty) > 256) {
    Serial.print("ERROR: Invalid Duty Value\n");
  } else {
    if(duty > 0) {
      analogWrite(vConf.oMotor1, duty);
      digitalWrite(vConf.oMotor2, LOW);
    } else {
      digitalWrite(vConf.oMotor1, LOW);
      analogWrite(vConf.oMotor2, abs(duty));
    }
    motor_speed = duty;
  }  
}

void force_on(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    Serial.print("Forcing Motor speed to ");
    Serial.print(STD_MOTOR_SPD * 100.0 / 1024.0);
    Serial.println("%...");
    set_motor_speed(STD_MOTOR_SPD);
    Serial.println("DONE");
    forceOverride = 1;
  }
}

void force_off(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    Serial.print("Forcing Motor Speed to 0%...");
    set_motor_speed(0);
    Serial.println("DONE\n");
    forceOverride = 1;
  }
}

void force_state_change(struct STATE_MACH *st, void *args, int argc, char *out) {
  if(st == NULL || argc > 1) {
    strcpy(out, "ERROR");
  } else {
    char argBuf[STD_BUF_SIZE];
    //TODO: get string from args, search stateNameArr for index, change st->curr
    strcpy(argBuf, args);

  
    int outInd = -1;
    for(int i = 0; i < END; i++) {
      if(strcmp(stateNameArr[i], argBuf) == 0) {
        outInd = i;
        break;
      }
    }
  
    if(outInd == -1) {
      strcpy(out, "ERROR");
    } else {
      st->curr = outInd;
  
      strcpy(out, stateNameArr[st->curr]);
    }
  }
}

void get_curr_state(struct STATE_MACH *st, void *args, int argc, char *out) {
  // NO OVERFLOW PROTECTION!!!
  
  if(st == NULL) {
    strcpy(out, "ERROR");
  } else {
    strcpy(out, stateNameArr[st->curr]);
  }
}
