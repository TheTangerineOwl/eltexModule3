#ifndef PROD_H
#define PROD_H
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <limits.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

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

#define MAX_SET_COUNT 1

#define FILEMODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

typedef struct sem_info
{
    sem_t* id;
    char* name;
} sem_info;

typedef struct shm_info
{
    int* shptr;
    int fd;
    size_t size;
    char* name;
} shm_info;

extern sem_info semap[3];
extern shm_info shmap[3];

#define MUTEX 0
#define EMPTY 1
#define FULL 2
#define MUTEX_SEM_NAME "/app11sem.mutex"
#define EMPTY_SEM_NAME "/app11sem.empty"
#define FULL_SEM_NAME "/app11sem.full"
#define SHARRAY 0
#define MINMAX 1
#define PROCOUNT 2
#define SHARRAY_SHM_NAME "/app11shm.sharray"
#define MINMAX_SHM_NAME "/app11shm.minmax"
#define PROCOUNT_SHM_NAME "/app11shm.count"

int* gen_rand_num_str(int* res_array, int* res_count);
int find_max_and_min(int* nums, int* max, int* min);

int init_sems(sem_info* sem);
void delete_sems(sem_info* sem);
int init_shm(shm_info* info, const char* name, size_t maxlen);
int init_all_shm(shm_info* shms);
void delete_all_shm(shm_info* shms);

int process_shm(sem_info* sem, shm_info* shms);
int produce_sharray(sem_info* sem, shm_info* shms);

#endif