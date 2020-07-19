#include "main.h"

int SerialBuffer[STD_BUF_SIZE];

void handle_serial_input(int c) {
  static int buf_write_pos = 0;

  if(c == 10 || c == 13) c = 0; //i dont want line feeds or carriage returns i want null terminators

  if(buf_write_pos > 1 && SerialBuffer[buf_write_pos - 1] == 0x0) {
    for(int i = buf_write_pos; i < STD_BUF_SIZE; i++) SerialBuffer[i] = -1;
  }

  SerialBuffer[buf_write_pos] = c;

  buf_write_pos++;
  if(buf_write_pos == STD_BUF_SIZE) {
    buf_write_pos = 0;

    //get the beginning of the word that started the wrap
    int endHolder = STD_BUF_SIZE;
    while(SerialBuffer[endHolder] != 0x0 && endHolder > 0) {
      endHolder--;
    }

    //clear from the front to the \0 before the next word
    for(int i = 0; i < endHolder; i++) {
      SerialBuffer[i] = -1;
    }
  }
}

int read_serial_buffer(char *out) {
  static int buf_read_pos = 0;

  int buf[STD_BUF_SIZE];
  int buf_ind;
  buf_ind = 0;
  int buf_read_pos_temp = buf_read_pos;

  if(SerialBuffer[buf_read_pos] == 0 || SerialBuffer[buf_read_pos] == -1) goto invalidErr;

  while(SerialBuffer[buf_read_pos_temp] != 0 && (buf_read_pos_temp - buf_read_pos) < STD_BUF_SIZE) {
    if(SerialBuffer[buf_read_pos_temp] == -1) goto invalidErr;
    buf[buf_ind] = SerialBuffer[buf_read_pos_temp];
    buf_read_pos_temp++;
    buf_ind++;
    if(buf_read_pos_temp == STD_BUF_SIZE) {
      buf_read_pos_temp = 0;
    }
  }

  buf[buf_ind++] = '\0';

  for(int i = 0; i < buf_ind; i++) {
    out[i] = char(buf[i]);
  }

  buf_read_pos_temp++;
  buf_read_pos = buf_read_pos_temp;
  return 1;

invalidErr: 
  out[0] = -1;
  return -1;
}

void printSerialBuffer() {
  for(int i = 0; i < STD_BUF_SIZE; i++) {
    Serial.print(SerialBuffer[i]);
    Serial.print(", ");
  }
  Serial.println();
}

int substr(int start, int _end, char *inp, char *o) {
    if(inp == NULL) return 0;
    int len = strlen(inp);
    if((_end - start) > len || _end > len || start < 0) return 0;
    if(start == _end) {
        *(o + 0) = inp[start];
    }

    int counter = start;
    while(counter != _end + 1) {
        *(o + counter - start) = inp[counter];
        counter++;
    }
    return 1;
}

int split_by_space(char *in, char *out1, char *out2) {
  if(in == NULL) {
    out1 = NULL;
    out2 = NULL;
    return 0;
  }

  int inLen = strlen(in);
  if(inLen <= 0) goto invalidErr;

  int stopPos = -1;
  for(int i = 0; i < inLen; i++) {
    if(in[i] == ' ') {
      stopPos = i;
      break;
    }
  }

  if(stopPos == -1) goto invalidErr;

  substr(0, stopPos - 1, in, out1);
  out1[stopPos] = '\0';
  substr(stopPos + 1, inLen - 1, in, out2);
  out2[inLen] = '\0';

  return 1;

invalidErr:
  strcpy(out1, in);
  out2 = NULL;
  return 0;
}
