#ifndef PROD_H
#define PROD_H
#define _POSIX_C_SOURCE 200809L

#define NUM_STR_LENGTH 11UL
#define MAX_GENERATED_LENGTH 200UL

#define GENERATOR_COUNT 20
#define GENERATOR_MAX 10000
#define GENERATOR_MIN -10000

#define FILE_MAX_LINE_COUNT 10
#define FILENAME "randnum.txt"
// #define FILENAME_TXT "rand_num.txt"

#define SEM_NAME_FILE "app09_fac_"
#define SEM_NAME_EMPTY "app09_emp_"
#define SEM_NAME_FULL "app09_ful_"

#define SEM_FLAGS O_CREAT
#define SEM_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

typedef struct sem_info
{
    sem_t* sem;
    char* sem_name;
} seminfo;

extern seminfo sinfo[3];
#define mutex sinfo[0].sem
#define empty sinfo[1].sem
#define full sinfo[2].sem
#define mutex_info sinfo[0]
#define empty_info sinfo[1]
#define full_info sinfo[2]
#define mutex_name sinfo[0].sem_name
#define empty_name sinfo[1].sem_name
#define full_name sinfo[2].sem_name

char* gen_rand_num_str(const int min, const int max);
int find_max_and_min(char* numstr, int* max, int* min);

sem_t* init_sem(seminfo* sem, const char* prefix, const char* filename, unsigned int value);
int process_file(const char* filename);
int append_file(const char* filename, const int min_num, const int max_num);

#endif