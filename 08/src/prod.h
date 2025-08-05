#ifndef PROD_H
#define PROD_H
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define ERROR(str) \
    { \
        perror(str); \
        return -1; \
    }

#define NUM_STR_LENGTH 11UL
#define MAX_GENERATED_LENGTH 200UL
#define FILE_MAX_LINE_COUNT 10
#define FILENAME "randnum.txt"

#define GENERATOR_COUNT 20
#define GENERATOR_MAX 10000
#define GENERATOR_MIN -10000

#define PATHNAME "/app08sem."
#define PROJ_ID 'B'

#define SEM_FLAGS O_CREAT
#define SEM_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

#define MUTEX_SEMNUM 0
#define EMPTY_SEMNUM 1
#define FULL_SEMNUM 2

typedef struct sem_info
{
    int semid;
    char* sem_name;
} sem_info;

union semun {
      int val;                  /* value for SETVAL */
      struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
      unsigned short *array;    /* array for GETALL, SETALL */
                                /* Linux specific part: */
      struct seminfo *__buf;    /* buffer for IPC_INFO */
};

char* gen_rand_num_str(const int min, const int max);
int find_max_and_min(char* numstr, int* max, int* min);

int init_sem(sem_info* sem, const char* prefix, const char* filename);
int delete_sems(sem_info* sem);
int capture_mutex(int semid);
int release_mutex(int semid);
int sem_add_str(int semid);
int sem_del_str(int semid);

int process_file(int semid, const char* filename);
int append_file(int semid, const char* filename, const int min_num, const int max_num);

#endif