#ifndef PROD_H
#define PROD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define ERROR(str, status) \
    { \
        perror(str); \
        return status ; \
    }

#define NUM_STR_LENGTH 11UL
#define MAX_GENERATED_LENGTH 200UL

#define GENERATOR_COUNT 20
#define GENERATOR_MAX 10000
#define GENERATOR_MIN -10000

// #define MAX_SET_COUNT 1

#define FILEMODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

union semun {
      int val;                  /* value for SETVAL */
      struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
      unsigned short *array;    /* array for GETALL, SETALL */
                                /* Linux specific part: */
      struct seminfo *__buf;    /* buffer for IPC_INFO */
};

typedef struct sem_info
{
    int id;
    char* name;
} sem_info;

typedef struct my_shm_info
{
    int* shptr;
    int id;
    size_t size;
    char* name;
} my_shm_info;

extern sem_info semap;
extern my_shm_info shmap[3];

#define MUTEX 0
#define EMPTY 1
#define FULL 2
#define DIR_NAME "tmp"
#define SEM_NAME "tmp/app10sem.semap"
#define PROJ_ID 'C'
#define SHARRAY 0
#define MINMAX 1
#define PROCOUNT 2
#define SHARRAY_SHM_NAME "tmp/app10shm.sharray"
#define MINMAX_SHM_NAME "tmp/app10shm.minmax"
#define PROCOUNT_SHM_NAME "tmp/app10shm.count"

int* gen_rand_num_str(int* res_array, int* res_count);
int find_max_and_min(int* nums, int* max, int* min);

void sem_shm_filetouch(void);

int init_sems(sem_info* sem);
void delete_sems(sem_info* sem);
int capture_mutex(int semid);
int release_mutex(int semid);
int sem_put_array(int semid);
int sem_read_array(int semid);

int init_shm(my_shm_info* info, const char* name, size_t maxlen);
int init_all_shm(my_shm_info* shms);
void delete_all_shm(my_shm_info* shms);

int process_shm(sem_info* sem, my_shm_info* shms);
int produce_sharray(sem_info* sem, my_shm_info* shms);

#endif