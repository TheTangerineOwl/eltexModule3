#include "prod.h"

// Файлы для генерации ключей
void sem_shm_filetouch()
{
    struct stat info;
    if (stat(DIR_NAME, &info) != 0 || !S_ISDIR(info.st_mode))
    {
        if (mkdir(DIR_NAME, FILEMODE) == -1)
        {
            fprintf(stderr, "Failed to find or create %s", DIR_NAME);
            perror("");
            exit(EXIT_FAILURE);
        }
    }

    FILE* fd;
    
    fd = fopen(SEM_NAME, "a");
    if (fd == NULL)
    {
        fprintf(stderr, "Failed to create file %s", SEM_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    fclose(fd);

    fd = fopen(SHARRAY_SHM_NAME, "a");
    if (fd == NULL)
    {
        fprintf(stderr, "Failed to create file %s", SHARRAY_SHM_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    fclose(fd);

    fd = fopen(MINMAX_SHM_NAME, "a");
    if (fd == NULL)
    {
        fprintf(stderr, "Failed to create file %s", MINMAX_SHM_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    fclose(fd);

    fd = fopen(PROCOUNT_SHM_NAME, "a");
    if (fd == NULL)
    {
        fprintf(stderr, "Failed to create file %s", PROCOUNT_SHM_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    fclose(fd);
}

int init_sems(sem_info* sem)
{
    sem->name = strdup(SEM_NAME);

    key_t key = ftok(SEM_NAME, PROJ_ID);
    if (key == -1)
        ERROR("init_sems (ftok)", -1);
    int semid = semget(key, 4, FILEMODE);
    if (semid == -1)
    {
        if (errno != ENOENT)
            ERROR("init_sems (semget old)", -1);
        semid = semget(key, 4, IPC_CREAT | FILEMODE);
        if (semid == -1)
            ERROR("init_sems (semget new)", -1);

        // mutex - 0, empty - 1, full - 2, processed - 3
        unsigned short arr[4];
        arr[MUTEX] = 1;
        arr[EMPTY] = 1;
        arr[FULL] = 0;
        arr[PROCESSED] = 0;
        union semun values = {.array = arr};
        if (semctl(semid, 4, SETALL, values) == -1)
        {
            delete_sems(sem);
            ERROR("semctl", -1);
        }
    }
    sem->id = semid;
    return sem->id;
}

void delete_sems(sem_info* sem)
{
    if (semctl(sem->id, 0, IPC_RMID, NULL) == -1)
    {
        perror("delete_sems");
        return;
    }
    if (sem->name)
        free(sem->name);
    sem->name = NULL;
}

int capture_mutex(int semid, int mutex_num)
{
    struct sembuf buf = {
        .sem_num = mutex_num,
        .sem_flg = 0,
        .sem_op = -1
    };
    if (semop(semid, &buf, 1) == -1)
        ERROR("capture_mutex", -1);
    return 0;
}

int release_mutex(int semid, int mutex_num)
{
    struct sembuf buf = {
        .sem_num = mutex_num,
        .sem_flg = 0,
        .sem_op = 1
    };
    if (semop(semid, &buf, 1) == -1)
        ERROR("release_mutex", -1);
    return 0;
}

// unlock on full (1), lock on empty (-1)
// (если empty==0, то блок)
int sem_put_array(int semid)
{
    struct sembuf buf[2] = {
        {FULL, 1, 0},
        {EMPTY, -1, 0}
    };
    if (semop(semid, buf, 2) == -1)
        ERROR("sem_put_array", -1);
    return 0;
}

// unlock on empty (1), lock on full (-1)
// (если full==0, то блок)
int sem_read_array(int semid)
{
    struct sembuf buf[2] = {
        {FULL, -1, 0},
        {EMPTY, 1, 0}
    };
    if (semop(semid, buf, 2) == -1)
        ERROR("sem_read_array", -1);
    return 0;
}

int init_shm(my_shm_info* info, const char* name, size_t maxlen)
{
    key_t key = ftok(name, PROJ_ID);
    if (key == -1)
        ERROR("init_shm (ftok)", -1);
    int shmid = shmget(key, maxlen, IPC_CREAT | IPC_EXCL | FILEMODE);
    if (shmid == -1)
    {
        if (errno != EEXIST)
        {
            ERROR("init_shm (shmget новый ключ)", -1);
        }
        else if ((shmid = shmget(key, maxlen, FILEMODE)) == -1)
            ERROR("init_shm (shmget существующий ключ)", -1);
    }
    info->id = shmid;
    info->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
    snprintf(info->name, strlen(name), "%s", name);
    info->name[strlen(name)] = '\0';
    info->size = maxlen;
    info->shptr = (int*)shmat(info->id, NULL, 0);
    if (info->shptr == (int*)-1)
        ERROR("init_shm (shmat)", -1);
    return 0;
}

int init_all_shm(my_shm_info* shms)
{
    size_t size = sizeof(int) * (GENERATOR_COUNT + 1);
    if (init_shm(&shms[SHARRAY], SHARRAY_SHM_NAME, size) == -1)
        ERROR("init_all_shm", -1);
    size = sizeof(int) * 2;
    if (init_shm(&shms[MINMAX], MINMAX_SHM_NAME, size) == -1)
        ERROR("init_all_shm", -1);
    size = sizeof(int);
    if (init_shm(&shms[PROCOUNT], PROCOUNT_SHM_NAME, size) == -1)
        ERROR("init_all_shm", -1);
    return 0;
}

void delete_all_shm(my_shm_info* shms)
{
    if (shms[SHARRAY].shptr)
    {
        if (shmctl(shms[SHARRAY].id, IPC_RMID, NULL) == -1)
            perror("delete_all_shm (shmctl RMID SHARRAY)");
        if (shmdt(shms[SHARRAY].shptr) == -1)
            perror("delete_all_shm (shmdt SHARRAY)");
        shms[SHARRAY].shptr = NULL;
        free(shms[SHARRAY].name);
    }
    if (shms[MINMAX].shptr)
    {
        if (shmctl(shms[MINMAX].id, IPC_RMID, NULL) == -1)
            perror("delete_all_shm (shmctl RMID MINMAX)");
        if (shmdt(shms[MINMAX].shptr) == -1)
            perror("delete_all_shm (shmdt MINMAX)");
        shms[MINMAX].shptr = NULL;
        free(shms[MINMAX].name);
    }
    if (shms[PROCOUNT].shptr)
    {
        if (shmctl(shms[PROCOUNT].id, IPC_RMID, NULL) == -1)
            perror("delete_all_shm (shmctl RMID PROCOUNT)");
        if (shmdt(shms[PROCOUNT].shptr) == -1)
            perror("delete_all_shm (shmdt PROCOUNT)");
        shms[PROCOUNT].shptr = NULL;
        free(shms[PROCOUNT].name);
    }
}

int process_shm(sem_info* sem, my_shm_info* shms)
{
    int* count, *max, *min, *processed, *nums;

    if (sem_read_array(sem->id) != 0)
        ERROR("process_shm (full P, empty V)", -1);
    
    if (capture_mutex(sem->id, MUTEX) != 0)
    {
        if (sem_put_array(sem->id) != 0)  // Откатываем sem_read_array
            perror("process_shm (full V, empty P)");
        ERROR("process_shm (mutex P)", -1);
    }

    nums = shms[SHARRAY].shptr;
    count = nums;
    min = &shms[MINMAX].shptr[0];
    max = &shms[MINMAX].shptr[1];
    processed = shms[PROCOUNT].shptr;

    if (find_max_and_min(shms[SHARRAY].shptr, max, min) == -1)
    {
        fprintf(stderr, "process_shm: find_max_and_min returned -1\n");
        if (release_mutex(sem->id, MUTEX) != 0)
            ERROR("process_shm (mutex V)", -1);
        if (sem_put_array(sem->id) != 0)
            ERROR("process_shm (empty V, full P)", -1);
        return -1;
    }
    // (*processed)++;
    printf("Child[%d]: min %d, max %d, sharray(%d): ", *processed, *min, *max, *count);
    for (int i = 1; i <= *count; i++)
        printf("%d ", nums[i]);
    puts("");
    fflush(stdout);

    if (release_mutex(sem->id, MUTEX) != 0)
        ERROR("process_shm (mutex V)", -1);

    if (sem_put_array(sem->id) != 0)
        ERROR("process_shm (empty V, full P)", -1);

    if (release_mutex(sem->id, PROCESSED) != 0)
        ERROR("process_shm (processed V)", -1);

    return 0;
}

int produce_sharray(sem_info* sem, my_shm_info* shms)
{
    if (capture_mutex(sem->id, MUTEX) != 0)
    {
        ERROR("produce_sharray (mutex P)", -1);
    }

    int* shar = shms[SHARRAY].shptr;
    int* count = shar;
    int* processed = shms[PROCOUNT].shptr;
    if (!gen_rand_num_str(shar, count))
    {
        fprintf(stderr, "produce_sharray: gen_rand_num_str returned -1\n");
        if (release_mutex(sem->id, MUTEX) != 0)
            ERROR("produce_sharray (full V, empty P)", -1);
        return -1;
    }
    int processed_buf = *processed;
    printf("Parent[%d]: sharray (count=%d): ", processed_buf, *count);
    for (int i = 1; i <= *count; i++)
        printf("%d ", shar[i]);
    puts("");
    fflush(stdout);

    if (release_mutex(sem->id, MUTEX) != 0)
        ERROR("produce_sharray (full V, empty P)", -1);

    printf("Parent[%d]: wrote array, waiting for min/max\n", processed_buf);
    fflush(stdout);

    if (sem_put_array(sem->id) != 0)
        ERROR("produce_sharray (full V, empty P)", -1);

    if (capture_mutex(sem->id, PROCESSED) != 0)
        ERROR("produce_sharray (processed P)", -1);

    if (sem_read_array(sem->id) != 0)
        ERROR("produce_sharray (empty P, full V)", -1);

    if (capture_mutex(sem->id, MUTEX) != 0) {
        ERROR("produce_sharray (mutex P)", -1);
    }

    int* max = &shms[MINMAX].shptr[1];
    int* min = &shms[MINMAX].shptr[0];
    printf("Parent[%d]: min = %d, max = %d.\n", processed_buf, *min, *max);
    fflush(stdout);
    (*processed)++;

    if (release_mutex(sem->id, MUTEX) != 0)
        ERROR("produce_sharray (full V, empty P)", -1);
    return 0;
}
