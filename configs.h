#ifndef __CONFIG_H__
#define __CONFIG_H__

typedef struct CONFIGS {
  int oMotor1;
  int oMotor2;
  int iPSens;
  int pSenseLow;
  int pSenseHigh;
  int pAttached;
  int reset_low;
  int reset_high;
  int reset_ext;
};

extern struct CONFIGS vConf;

#endif
