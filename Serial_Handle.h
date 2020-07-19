#ifndef __SERIAL_HANDLE_H__
#define __SERIAL_HANDLE_H__

extern int SerialBuffer[STD_BUF_SIZE];

void handle_serial_input(int c);
int read_serial_buffer(char *out);
void printSerialBuffer();
int substr(int start, int _end, char *inp, char *o);
int split_by_space(char *in, char *out1, char *out2);


#endif
