#include "prod.h"

int init_sems(sem_info* sem)
{
    sem[MUTEX].name = MUTEX_SEM_NAME;
    sem[MUTEX].id = sem_open(MUTEX_SEM_NAME, O_CREAT, FILEMODE, 1);
    if (sem[MUTEX].id == SEM_FAILED)
        ERROR("init_sems (mutex)", -1);
    sem[EMPTY].name = EMPTY_SEM_NAME;
    sem[EMPTY].id = sem_open(EMPTY_SEM_NAME, O_CREAT, FILEMODE, MAX_SET_COUNT);
    if (sem[EMPTY].id == SEM_FAILED)
        ERROR("init_sems (empty)", -1);
    sem[FULL].name = FULL_SEM_NAME;
    sem[FULL].id = sem_open(FULL_SEM_NAME, O_CREAT, FILEMODE, 0);
    if (sem[FULL].id == SEM_FAILED)
        ERROR("init_sems (full)", -1);
    return 0;
}

void delete_sems(sem_info* sem)
{
    sem_close(sem[MUTEX].id);
    sem_unlink(sem[MUTEX].name);
    sem_close(sem[EMPTY].id);
    sem_unlink(sem[EMPTY].name);
    sem_close(sem[FULL].id);
    sem_unlink(sem[FULL].name);
}

int init_shm(shm_info* info, const char* name, size_t maxlen)
{
    if (!info || !name)
        return -1;
    info->fd = shm_open(name, O_CREAT | O_RDWR, FILEMODE);
    if (info->fd == -1)
        ERROR("init_shm (shm_open)", -1);
    if (ftruncate(info->fd, maxlen) == -1)
        ERROR("init_shm (ftruncate)", -1);
    info->shptr = (int*)mmap(NULL, maxlen, PROT_READ | PROT_WRITE, MAP_SHARED, info->fd, 0);
    if (info->shptr == MAP_FAILED)
        ERROR("init_shm (mmap)", -1);
    info->name = strdup(name);
    info->size = maxlen;
    return 0;
}

int init_all_shm(shm_info* shms)
{
    size_t size = sizeof(int) * (GENERATOR_COUNT + 1) * MAX_SET_COUNT;
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

void delete_all_shm(shm_info* shms)
{
    if (shms[SHARRAY].shptr)
    {
        if (munmap(shms[SHARRAY].shptr, shms[SHARRAY].size) == -1)
            ERROR("delete_all_shm (munmap SHARRAY)", );
        shm_unlink(shms[SHARRAY].name);
        shms[SHARRAY].shptr = NULL;
        free(shms[SHARRAY].name);
    }
    if (shms[MINMAX].shptr)
    {
        if (munmap(shms[MINMAX].shptr, shms[MINMAX].size) == -1)
            ERROR("delete_all_shm (munmap MINMAX)", );
        shm_unlink(shms[MINMAX].name);
        shms[MINMAX].shptr = NULL;
        free(shms[MINMAX].name);
    }
    if (shms[PROCOUNT].shptr)
    {
        if (munmap(shms[PROCOUNT].shptr, shms[PROCOUNT].size) == -1)
            ERROR("delete_all_shm (munmap PROCOUNT)", );
        shm_unlink(shms[PROCOUNT].name);
        shms[PROCOUNT].shptr = NULL;
        free(shms[PROCOUNT].name);
    }
}

int process_shm(sem_info* sem, shm_info* shms)
{
    int* count, *max, *min, *processed, *nums;
    if (sem_wait(sem[FULL].id) != 0)
        ERROR("process_shm (full P)", -1);
    if (sem_wait(sem[MUTEX].id) != 0)
    {
        if (sem_post(sem[FULL].id) != 0)
            perror("process_shm (full V)");
        ERROR("process_shm (mutex P)", -1);
    }

    nums = shms[SHARRAY].shptr;
    count = nums;
    min = &shms[MINMAX].shptr[0];
    max = &shms[MINMAX].shptr[1];
    processed = &shms[PROCOUNT].shptr[0];

    if (find_max_and_min(shms[SHARRAY].shptr, max, min) == -1)
    {
        fprintf(stderr, "process_shm: find_max_and_min returned -1\n");
        if (sem_post(sem[MUTEX].id) != 0)
        {
            if (sem_post(sem[EMPTY].id) != 0)
                perror("process_shm (empty V)");
            ERROR("process_shm (mutex V)", -1);
        }
        if (sem_post(sem[EMPTY].id) != 0)
            ERROR("process_shm (empty V)", -1);
        return -1;
    }
    // (*processed)++;
    printf("Child[%d]: min %d, max %d, shar(%d): ", *processed, *min, *max, *count);
    for (int i = 1; i <= *count; i++)
        printf("%d ", nums[i]);
    puts("");
    fflush(stdout);

    if (sem_post(sem[MUTEX].id) != 0)
    {
        if (sem_post(sem[EMPTY].id) != 0)
            perror("process_shm (empty V)");
        ERROR("process_shm (mutex V)", -1);
    }
    if (sem_post(sem[EMPTY].id) != 0)
        ERROR("process_shm (empty V)", -1);

    return 0;
}

int produce_sharray(sem_info* sem, shm_info* shms)
{
    if (sem_wait(sem[EMPTY].id) != 0)
        ERROR("produce_sharray (empty P)", -1);
    if (sem_wait(sem[MUTEX].id) != 0)
    {
        if (sem_post(sem[EMPTY].id) != 0)
            perror("produce_sharray (empty V)");
        ERROR("produce_sharray (mutex P)", -1);
    }

    int* shar = shms[SHARRAY].shptr;
    int* count = shar;
    int* processed = shms[PROCOUNT].shptr;
    if (!gen_rand_num_str(shar, count))
    {
        fprintf(stderr, "produce_sharray: gen_rand_num_str returned -1\n");
        if (sem_post(sem[MUTEX].id) != 0)
        {
            if (sem_post(sem[FULL].id) != 0)
                perror("produce_sharray (full V)");
            ERROR("produce_sharray (mutex V)", -1);
        }
        if (sem_post(sem[FULL].id) != 0)
            ERROR("produce_sharray (full V)", -1);
        return -1;
    }
    int processed_buf = *processed;
    printf("Parent[%d]: sharray (count=%d): ", processed_buf, *count);
    for (int i = 1; i <= *count; i++)
        printf("%d ", shar[i]);
    puts("");
    fflush(stdout);

    if (sem_post(sem[MUTEX].id) != 0)
    {
        if (sem_post(sem[FULL].id) != 0)
            perror("produce_sharray (full V)");
        ERROR("produce_sharray (mutex V)", -1);
    }
    if (sem_post(sem[FULL].id) != 0)
        ERROR("produce_sharray (full V)", -1);

    printf("Parent[%d]: wrote array, waiting for min/max\n", processed_buf);
    fflush(stdout);

    if (sem_wait(sem[EMPTY].id) != 0)
        ERROR("produce_sharray (empty P)", -1);
    if (sem_wait(sem[MUTEX].id) != 0)
    {
        if (sem_post(sem[EMPTY].id) != 0)
            perror("produce_sharray (empty V)");
        ERROR("produce_sharray (mutex P)", -1);
    }

    int* max = &shms[MINMAX].shptr[1];
    int* min = &shms[MINMAX].shptr[0];
    printf("Parent[%d]: min = %d, max = %d.\n", processed_buf, *min, *max);
    (*processed)++;

    if (sem_post(sem[MUTEX].id) != 0)
    {
        if (sem_post(sem[FULL].id) != 0)
            perror("produce_sharray (full V)");
        ERROR("produce_sharray (mutex V)", -1);
    }
    if (sem_post(sem[FULL].id) != 0)
        ERROR("produce_sharray (full V)", -1);
    return 0;
}