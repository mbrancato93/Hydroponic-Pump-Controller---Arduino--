#include "mlist.h"

int list_init(struct STATE_NODE **h, state _sName, int _d[4], void (*_f)(int a[4], int, int)) {
  if(*h != NULL) return -1;

  *h = malloc(sizeof(struct STATE_NODE));
  if(*h == NULL) return -1;

  (*h)->next = NULL;
  (*h)->sName = _sName;
  for(int i = 0; i < 4; i++) {
    (*h)->d[i] = _d[i];
  }
  (*h)->f = _f;

  return 1;
}

int list_push_node(struct STATE_NODE *h, struct STATE_NODE *node) {
  if(h == NULL) {
    list_init(&h, node->sName, node->d, node->f);
    return 2;
  } else {
    struct STATE_NODE *tmp;
    tmp = h;
    while(tmp->next != NULL) {
      tmp = tmp->next;
    }
    tmp->next = node;
    node->next = NULL;
    return 1;
  }
  return -1;
}

int list_push_val(struct STATE_NODE *h, state _sName, int _d[4], void (*_f)(int a[4], int, int)) {
  struct STATE_NODE *tmp;
  tmp = malloc(sizeof(struct STATE_NODE));
  if(tmp == NULL) return -1;

  tmp->next = NULL;
  tmp->sName = _sName;
  for(int i = 0; i < 4; i++) {
    tmp->d[i] = _d[i];
  }
  tmp->f = _f;
  return list_push_node(h, tmp);
}

int list_size(struct STATE_NODE *h) {
  if(h == NULL) return -1;

  struct STATE_NODE *tmp;
  tmp = h;

  int counter = 1;
  while(tmp->next != NULL) {
    counter++;
    tmp = tmp->next;
  }
  return counter;
}

void boot_func(int _d[4], int _mSpeed, int _pVal) {
  //do nothing...
}

void lowpressure_func(int _d[4], int _mSpeed, int _pVal) {
  //low pressure
  set_motor_speed(((float)_d[3] + (float)_d[2])*(2.56/2.0));
}

void highpressure_func(int _d[4], int _mSpeed, int _pVal) {
  //high pressure
  set_motor_speed(((float)_d[3] + (float)_d[2])*(2.56/2.0));
}

void standard_func(int _d[4], int _mSpeed, int _pVal) {
  //standard operation
  set_motor_speed(((float)_d[3] + (float)_d[2])*(2.56/2.0)); //for now, average the range given for motor speeds
}

void off_func(int _d[4], int _mSpeed, int _pVal) {
  //off
  printf("\nOFF state. Verify no blockage. Restart Requred\n");
  set_motor_speed(((float)_d[3] + (float)_d[2])*(2.56/2.0));
}
