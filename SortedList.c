#include "SortedList.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int opt_yield;
void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
  SortedList_t* temp = list;
  while (temp->next->key) {
    if (*(temp->next->key) > *(element->key)) {
      break;
    }
    temp = temp->next;
  }
  element->prev = temp;
  element->next = temp->next;
  if (opt_yield & INSERT_YIELD) {
    sched_yield();
  }
  temp->next = element;
  element->next->prev = element;
}

int SortedList_delete( SortedListElement_t *element) {
  if (element->next->prev == element && element->prev->next == element) {
    element->prev->next = element->next;
    if (opt_yield & DELETE_YIELD) {
      sched_yield();
    }
    element->next->prev = element->prev;
    return 0;
  }
  return 1;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
  SortedList_t* temp = list->next;
  SortedList_t* ret = NULL;
  while (temp->key) {
    if (*(temp->key) == *key) {
      ret = temp;
      break;
    }
    if (opt_yield & LOOKUP_YIELD) {
      sched_yield();
    }
    temp = temp->next;
  }
  return ret;
}

int SortedList_length(SortedList_t *list) {
  int len = 0;
  SortedList_t* temp = list->next;
  while (temp->key) {
    if (temp->prev->next == temp && temp->next->prev == temp)
      len += 1;
    else
      return -1;
    if (opt_yield & LOOKUP_YIELD) {
      sched_yield();
    }
    temp = temp->next;
  }
  if (list->prev->next == list && list->next->prev == list)
    return len;
  else
    return -1;
}
