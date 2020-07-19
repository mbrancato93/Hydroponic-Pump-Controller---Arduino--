#ifndef __HYDRO_CMDS__
#define __HYDRO_CMDS__

#include "main.h"

#define STD_MOTOR_SPEED 900

typedef struct cmd_entry {
  char cmd[15];
  void (*f)(struct STATE_MACH *, void *, int, char *);
};


extern void(* resetFunc) (void);

void reset_func(struct STATE_MACH *st, void *args, int argc, char *out);
void help_func(struct STATE_MACH *st, void *args, int argc, char *out);
void set_motor_speed(int duty);
void force_on(struct STATE_MACH *st, void *args, int argc, char *out);
void force_off(struct STATE_MACH *st, void *args, int argc, char *out);
void force_state_change(struct STATE_MACH *st, void *args, int argc, char *out);
void get_curr_state(struct STATE_MACH *st, void *args, int argc, char *out);
void printConfig(struct STATE_MACH *st, void *args, int argc, char *out);
void setEEPROMvar(struct STATE_MACH *st, void *args, int argc, char *out);
void getEEPROMval(struct STATE_MACH *st, void *args, int argc, char *out);
void getSpeeda(struct STATE_MACH *st, void *args, int argc, char *out);
int getSpeedi();
int getPReading(struct STATE_MACH *m);
void getPressure(struct STATE_MACH *st, void *args, int argc, char *out);
void print_eeVarArr(struct STATE_MACH *st, void *args, int argc, char *out);
void print_extReset_val(struct STATE_MACH *st, void *args, int argc, char *out);

extern int cmd_arr_size;
extern struct cmd_entry cmd_arr[];
extern int forceOverride;

#include "main.h"


#endif
