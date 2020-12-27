
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include "SortedList.h"
#include <signal.h>

pthread_mutex_t* locks = NULL;
int* spin_locks = NULL;

int num_threads = 1;
int num_iter = 1;
int list_num = 1;
SortedList_t** sub_lists = NULL;
SortedList_t* elements = NULL;
int opt_yield;
int mutex = 0;
int spin = 0;
char* arr = NULL;
long* accumulators = NULL;

void sig_handler(int sig) {
  fprintf(stderr, "Caught signal %d\n", sig);
  exit(1);
}

void* list_wrap(void* input) {
  int* start;
  start = (int *) input;
  int offset = (*start) * num_iter;
  struct timespec t_start;
  struct timespec t_end;
  for (int i = 0; i < num_iter; i++) {
    if (mutex != 0) {
      int l_index = *(elements[offset+i].key) % list_num;
      if (clock_gettime(CLOCK_MONOTONIC, &t_start) < 0) {
	fprintf(stderr, "Getting system time failed!\n");
	exit(1);
      }
      pthread_mutex_lock(&locks[l_index]);
      if (clock_gettime(CLOCK_MONOTONIC, &t_end) < 0) {
	fprintf(stderr, "Getting system time failed!\n");
	exit(1);
      }
      accumulators[(*start)] += (t_end.tv_sec - t_start.tv_sec) * 1e9 + (t_end.tv_nsec - t_start.tv_nsec);
      SortedList_insert(sub_lists[l_index], &elements[offset+i]);
      pthread_mutex_unlock(&locks[l_index]);
    }
    else if (spin != 0) {
      int l_index = *(elements[offset+i].key) % list_num;
      while (__sync_lock_test_and_set(&spin_locks[l_index], 1))
	;
      SortedList_insert(sub_lists[l_index], &elements[offset+i]);
      __sync_lock_release(&spin_locks[l_index]);
    }
    else {
      int l_index = *(elements[offset+i].key) % list_num;
      SortedList_insert(sub_lists[l_index], &elements[offset+i]);
    }
  }

  if (mutex != 0) {
    for (int i = 0; i < list_num; i++) {
      if (clock_gettime(CLOCK_MONOTONIC, &t_start) < 0) {
	fprintf(stderr, "Getting system time failed!\n");
	exit(1);
      }
      pthread_mutex_lock(&locks[i]);
      if (clock_gettime(CLOCK_MONOTONIC, &t_end) < 0) {
	fprintf(stderr, "Getting system time failed!\n");
	exit(1);
      }
      accumulators[(*start)] += (t_end.tv_sec - t_start.tv_sec) * 1e9 + (t_end.tv_nsec - t_start.tv_nsec);
      if (SortedList_length(sub_lists[i]) < 0) {
	fprintf(stderr, "List has been corrupted!\n");
	exit(2);
      }
      pthread_mutex_unlock(&locks[i]);
    }
  }
  else if (spin != 0) {
    for (int i = 0; i < list_num; i++) {
      while (__sync_lock_test_and_set(&spin_locks[i], 1))
	;
      if (SortedList_length(sub_lists[i]) < 0) {
	fprintf(stderr, "List has been corrupted!\n");
	exit(2);
      }
      __sync_lock_release(&spin_locks[i]);
    }
  }
  else {
    for (int i = 0; i < list_num; i++) {
      if (SortedList_length(sub_lists[i]) < 0) {
	fprintf(stderr, "List has been corrupted!\n");
	exit(2);
      }
    }
  }

  for (int i = 0; i < num_iter; i++) {
    if (mutex != 0) {
      int l_index = *(elements[offset+i].key) % list_num;
      if (clock_gettime(CLOCK_MONOTONIC, &t_start) < 0) {
	fprintf(stderr, "Getting system time failed!\n");
	exit(1);
      }
      pthread_mutex_lock(&locks[l_index]);
      if (clock_gettime(CLOCK_MONOTONIC, &t_end) < 0) {
	fprintf(stderr, "Getting system time failed!\n");
	exit(1);
      }
      accumulators[(*start)] += (t_end.tv_sec - t_start.tv_sec) * 1e9 + (t_end.tv_nsec - t_start.tv_nsec);
      SortedList_t* delete;
      delete = SortedList_lookup(sub_lists[l_index], elements[offset+i].key);
      if (delete) {
	if (SortedList_delete(delete) != 0) {
	  fprintf(stderr, "List has been corrupted!\n");
	  exit(2);
	}
      }
      else {
	fprintf(stderr, "Can't find inserted element!\n");
	exit(2);
      }
      pthread_mutex_unlock(&locks[l_index]);
    }
    else if (spin != 0) {
      int l_index = *(elements[offset+i].key) % list_num;
      while (__sync_lock_test_and_set(&spin_locks[l_index], 1))
	;
      SortedList_t* delete;
      delete = SortedList_lookup(sub_lists[l_index], elements[offset+i].key);
      if (delete) {
	if (SortedList_delete(delete) != 0) {
	  fprintf(stderr, "List has been corrupted!\n");
	  exit(2);
	}
      }
      else {
	fprintf(stderr, "Can't find inserted element!\n");
	exit(2);
      }
      __sync_lock_release(&spin_locks[l_index]);
    }
    else {
      int l_index = *(elements[offset+i].key) % list_num;
      SortedList_t* delete;
      delete = SortedList_lookup(sub_lists[l_index], elements[offset+i].key);
      if (delete) {
	int del_val;
	del_val = SortedList_delete(delete);
	if (del_val != 0) {
	  fprintf(stderr, "List has been corrupted!\n");
	  exit(2);
	}
      }
      else {
	fprintf(stderr, "Can't find inserted element!\n");
	exit(2);
      }
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  signal(SIGSEGV, sig_handler);
  int c;
  char s[100] = "list-";
  long time_diff;
  long ltime = 0;
  int key[3] = {0, 0, 0};
  char* l_ = "l";
  char* i_ = "i";
  char* d_ = "d";
  int rc;
  int i;
  struct timespec time_start;
  struct timespec time_end;
  while(1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", required_argument, 0, 'y'},
      {"sync", required_argument, 0, 's'},
      {"lists", required_argument, 0, 'l'},
      {0, 0, 0, 0}};
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 't':
      num_threads = atoi(optarg);
      break;
    case 'i':
      num_iter = atoi(optarg);
      break;
    case 'y':
      if (strlen(optarg) > 3) {
	fprintf(stderr, "Invalid argument passed to yield!\n");
	exit(1);
      }
      if (strchr(optarg, 'i') != NULL) {
	opt_yield = opt_yield | INSERT_YIELD;
	key[0] = 'i';
      }
      if (strchr(optarg, 'd') != NULL) {
	opt_yield = opt_yield | DELETE_YIELD;
	key[1] = 'd';
      }
      if (strchr(optarg, 'l') != NULL) {
	opt_yield = opt_yield | LOOKUP_YIELD;
	key[2] = 'l';
      }
      break;
    case 'l':
      list_num = atoi(optarg);
      break;
    case 's':
      if (strlen(optarg) > 1) {
	fprintf(stderr, "Incorrect argument to sync option!\n");
	exit(1);
      }
      if (*optarg == 'm')
	mutex = 1;
      else if (*optarg == 's')
	spin = 1;
      else {
	fprintf(stderr, "Incorrect argument to sync option!\n");
	exit(1);
      }
      break;
    case '?':
      fprintf(stderr, "Unknown argument passed!\n");
      exit(1);
    default:
      break;
    }
  }
  sub_lists = (SortedList_t**) malloc(list_num * sizeof(SortedList_t *));
  SortedList_t lists[list_num];
  for (i = 0; i < list_num; i++) {
    sub_lists[i] = &lists[i];
    sub_lists[i]->prev = sub_lists[i];
    sub_lists[i]->next = sub_lists[i];
    sub_lists[i]->key = NULL;
  }

  locks = (pthread_mutex_t*) malloc(list_num * sizeof(pthread_mutex_t));
  spin_locks = (int *) malloc(list_num * sizeof(int));

  arr = (char *) malloc(num_iter * num_threads * sizeof(char));
  elements = (SortedList_t *) malloc(num_iter * num_threads * sizeof(SortedList_t));
  for (i = 0; i < (num_iter * num_threads); i++) {
    arr[i] = 97 + (rand() % 26);
    elements[i].key = &arr[i];
  }

  accumulators = (long *) malloc(num_threads * sizeof(long));
  for (i = 0; i < num_threads; i++)
    accumulators[i] = 0;

  pthread_t threads[num_threads];
  int* thread_nums = (int*) malloc(num_threads * sizeof(int));
  for (i = 0; i < num_threads; i++)
    thread_nums[i] = i;

  if (mutex != 0) {
    for (i = 0; i < list_num; i++) {
      if (pthread_mutex_init(&locks[i], NULL) != 0) {
	fprintf(stderr, "Mutex init failed!\n");
	exit(1);
      }
    }
  }

  if (clock_gettime(CLOCK_MONOTONIC, &time_start) < 0) {
    fprintf(stderr, "Getting system time failed!\n");
    exit(1);
  }

  for (i = 0; i < num_threads; i++) {
    rc = pthread_create(&threads[i], NULL, list_wrap, (void *) &(thread_nums[i]));
    if (rc) {
      fprintf(stderr, "Error with creating a thread!");
      exit(1);
    }
  }

  for (i = 0; i < num_threads; i++) {
    rc = pthread_join(threads[i], NULL);
    if (rc) {
      fprintf(stderr, "Error with retrieving a thread!");
      exit(1);
    }
  }
  
  if (clock_gettime(CLOCK_MONOTONIC, &time_end) < 0) {
    fprintf(stderr, "Getting system time failed!\n");
    exit(1);
  }

  for (int i = 0; i < list_num; i++) {
    if (SortedList_length(sub_lists[i]) < 0) {
      fprintf(stderr, "List has been corrupted!\n");
      exit(2);
    }
  }
  
  free(arr);
  free(elements);
  free(thread_nums);
  free(spin_locks);
  free(sub_lists);
  for (i = 0; i < list_num; i++)
    pthread_mutex_destroy(&locks[i]);
  free(locks);

  if (key[0] != 0)
    strcat(s, i_);
  if (key[1] != 0)
    strcat(s, d_);
  if (key[2] != 0)
    strcat(s, l_);
  if (strlen(s) == 5)
    strcat(s, "none");

  strcat(s, "-");

  if (mutex != 0)
    strcat(s, "m");
  else if (spin != 0)
    strcat(s, "s");
  else
    strcat(s, "none");

  time_diff = time_end.tv_sec - time_start.tv_sec;
  time_diff *= 1e9;
  time_diff += (time_end.tv_nsec - time_start.tv_nsec);

  for (i = 0; i < num_threads; i++) {
    ltime += accumulators[i];
  }
  ltime = ltime / (2*num_threads*num_iter);

  printf("%s,%d,%d,%d,%d,%ld,%ld,%ld\n", s, num_threads, num_iter, list_num, 3*num_threads*num_iter, time_diff, (time_diff)/(3*num_threads*num_iter), ltime);
  free(accumulators);
  exit(0);

}
