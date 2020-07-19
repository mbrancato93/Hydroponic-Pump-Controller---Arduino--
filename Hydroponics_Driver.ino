#include "main.h"

char stateNameArr[][STD_BUF_SIZE] = {"LOW PRESSURE", "HIGH PRESSURE", "STANDARD", "BOOT", "OFF"};

#define MOTOR1_EEADDR 0
#define MOTOR2_EEADDR MOTOR1_EEADDR + 1 * sizeof(int)
#define PSENS_EEADDR MOTOR1_EEADDR + 2 * sizeof(int)
#define PSENSLOW_EEADDR MOTOR1_EEADDR + 3 * sizeof(int)
#define PSENSHIGH_EEADDR MOTOR1_EEADDR + 4 * sizeof(int)
#define PATTACHED_EEADDR MOTOR1_EEADDR + 5 * sizeof(int)
#define RESETLOW_EEADDR MOTOR1_EEADDR + 6 * sizeof(int)
#define RESETHIGH_EEADDR MOTOR1_EEADDR + 7 * sizeof(int)
#define RESETEXT_EEADDR MOTOR1_EEADDR + 8 * sizeof(int)

int EE_LOC_size = 9;
struct _eeLookup EE_LOC[] {
  {"M1", MOTOR1_EEADDR},
  {"M2", MOTOR2_EEADDR},
  {"PSENS", PSENS_EEADDR},
  {"PSENSLOW", PSENSLOW_EEADDR},
  {"PSENSHIGH", PSENSHIGH_EEADDR},
  {"PATTACHED", PATTACHED_EEADDR},
  {"RESETLOW", RESETLOW_EEADDR},
  {"RESETHIGH", RESETHIGH_EEADDR},
  {"RESET_EXT", RESETEXT_EEADDR}
};

struct STATE_MACH mState;

unsigned long lastStateChange;
int offFlag = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println();
  Serial.print("Initializing Config Variables...");
  EEPROM.get(MOTOR1_EEADDR, vConf.oMotor1);
  EEPROM.get(MOTOR2_EEADDR, vConf.oMotor2);
  EEPROM.get(PSENS_EEADDR, vConf.iPSens);
  EEPROM.get(PSENSLOW_EEADDR, vConf.pSenseLow);
  EEPROM.get(PSENSHIGH_EEADDR, vConf.pSenseHigh);
  EEPROM.get(PATTACHED_EEADDR, vConf.pAttached);
  EEPROM.get(RESETLOW_EEADDR, vConf.reset_low);
  EEPROM.get(RESETHIGH_EEADDR, vConf.reset_high);
  EEPROM.get(RESETEXT_EEADDR, vConf.reset_ext);
  Serial.print("DONE\n");


  char buf[256];
  sprintf(buf, "Initializing State Machine with %d States...", END);
  Serial.print(buf);
  
  mState.curr = BOOT;
  int mStateArr[] = {0, 0, 0, 0};
  list_init(&mState.head, BOOT, mStateArr, boot_func);

  int low_pressure_arr[] = {0, 100, 90, 100};
  list_push_val(mState.head, LOW_PRESSURE, low_pressure_arr, lowpressure_func);

  int high_pressure_arr[] = {106, 120, 0, 75};
  list_push_val(mState.head, HIGH_PRESSURE, high_pressure_arr, highpressure_func);

  int standard_arr[] = {101, 105, 89, 75};
  list_push_val(mState.head, STANDARD, standard_arr, standard_func);

  int off_arr[] = {121, 1024, 0, 0};
  list_push_val(mState.head, OFF, off_arr, off_func);

  Serial.print("DONE [SIZE: ");
  Serial.print(list_size(mState.head));
  Serial.println("]");

  Serial.print("Setting Pin States...");
  pinMode(vConf.oMotor1, OUTPUT);
  pinMode(vConf.oMotor2, OUTPUT);
  pinMode(vConf.iPSens, INPUT);
  pinMode(vConf.reset_ext, OUTPUT);
  Serial.print("DONE\n");

  Serial.println("Booting Complete...Changing State to 'STANDARD'");
  mState.curr = STANDARD;
  lastStateChange = millis();
  offFlag = 0; //forces manual reset after off setting. This means i have to check to make sure there isnt a blockage

  Serial.print(">>");

  memset(SerialBuffer, -1, STD_BUF_SIZE * sizeof(int));
}

void handleState(struct STATE_MACH *m, int mSpeed, int pVal) {
  struct STATE_NODE *h = m->head;

  while(h != NULL) {
    if(pVal > h->d[0] && pVal < h->d[1]) {
      if(m->curr != h->sName) {
        Serial.print("Status change from ");
        Serial.print(stateNameArr[m->curr]);
        Serial.print(" to ");
        m->curr = h->sName;
        Serial.print(stateNameArr[m->curr]);
        Serial.print(" [");
        Serial.print(pVal);
        Serial.println("]");
      }
      break;
    }
    h = h->next;
  }
}

void currStateFunc(struct STATE_MACH *m, int mSpeed, int pVal) {
  struct STATE_NODE *h = m->head;

  while(h != NULL) {
    if(mState.curr == h->sName) {
      int _d[4];
      for(int i = 0; i < 4; i++) {
        _d[i] = h->d[i];
      }
      h->f(_d, mSpeed, pVal);
      break;
    }
    h = h->next;
  }
}

void externalResetButtonWatchdog() {
  const long minHoldTime = 1000;

  static long lastLow = -1;

  long currTime = millis();

  int resetRead = analogRead(vConf.reset_ext);
  if(resetRead < vConf.reset_low) { //this setup helps prevent continual resetting if button is never connected
    if(lastLow != -1 && (currTime - lastLow) > minHoldTime) {
      Serial.println("External Reset Button Triggered...");
      reset_func(&mState, NULL, 1, NULL);
    }
  } else {
    lastLow = millis();
  }
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  externalResetButtonWatchdog();
  
  if(Serial.available() > 0) {
    handle_serial_input(Serial.read());

    char holder[STD_BUF_SIZE];
    memset(holder, -1, sizeof(holder));
    int outter = read_serial_buffer(holder);
    if(outter > 0) {
      Serial.println(holder);

      // printSerialBuffer();

      char cmdBuf[STD_BUF_SIZE];
      char argBuf[STD_BUF_SIZE];
      memset(cmdBuf, 0x0, STD_BUF_SIZE);
      memset(argBuf, 0x0, STD_BUF_SIZE);
      split_by_space(holder, cmdBuf, argBuf);

      char outBuf[STD_BUF_SIZE];
      memset(outBuf, 0x0, STD_BUF_SIZE * sizeof(char));
      for(int j = 0; j < cmd_arr_size + 1; j++) {
        if(strcmp(cmdBuf, cmd_arr[j].cmd) == 0) {
          cmd_arr[j].f(&mState, argBuf, 1, outBuf);
          Serial.println(outBuf);
          break;
        } else if(j == cmd_arr_size) {
          Serial.println("Unknown command...");
        }
      }
      Serial.print(">>");
    }
  }

  if((millis() - lastStateChange) > 5000 && !offFlag && !forceOverride) {
    if(mState.curr == OFF) offFlag = 1;
    int pVal = getPReading(&mState);
    handleState(&mState, getSpeedi(), pVal);
    currStateFunc(&mState, getSpeedi(), pVal);
    lastStateChange = millis();
  }

  delay(10);
}
