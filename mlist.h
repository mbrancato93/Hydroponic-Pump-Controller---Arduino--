#ifndef __MLIST_H__
#define __MLIST_H__

#include "main.h"

typedef struct STATE_NODE {
  state sName;
  int d[4];
  void (*f)(int _d[4], int, int);
  struct STATE_NODE *next;
};

typedef struct STATE_MACH {
  state curr;
  struct STATE_NODE *head;
};


//this block modified only from mlist.old
int list_init(struct STATE_NODE **h, state _sName, int _d[4], void (*_f)(int a[4], int, int));
int list_push_node(struct STATE_NODE *h, struct STATE_NODE *node);
int list_push_val(struct STATE_NODE *h, state _sName, int _d[4], void (*_f)(int a[4], int, int));
int list_size(struct STATE_NODE *h);

void standard_func(int _d[4], int _mSpeed, int _pVal);
void boot_func(int _d[4], int _mSpeed, int _pVal);
void lowpressure_func(int _d[4], int _mSpeed, int _pVal);
void highpressure_func(int _d[4], int _mSpeed, int _pVal);
void off_func(int _d[4], int _mSpeed, int _pVal);

#endif
