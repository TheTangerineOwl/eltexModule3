#include "prod.h"

int init_sem(sem_info* sem, const char* prefix, const char* filename)
{
    char sem_name[FILENAME_MAX];
    strcat(sem_name, prefix);
    strcat(sem_name, filename);
    sem->sem_name = strdup(sem_name);

    key_t key = ftok(sem_name, PROJ_ID);
    int semid = semget(key, 3, SEM_MODE);
    if (semid == -1)
    {
        if (errno != ENOENT)
            ERROR("semget (attach old)");
        semid = semget(key, 3, IPC_CREAT | SEM_MODE);
        if (semid == -1)
            ERROR("semget (create new)");
        
        // mutex - 0, empty - 1, full 2
        unsigned short arr[3];
        arr[MUTEX_SEMNUM] = 1;
        arr[EMPTY_SEMNUM] = FILE_MAX_LINE_COUNT;
        arr[FULL_SEMNUM] = 0;
        union semun values = {.array = arr};
        if (semctl(semid, 3, SETALL, values) == -1)
        {
            delete_sems(sem);
            ERROR("semctl");
        }
    }
    sem->semid = semid;
    return sem->semid;
}

int delete_sems(sem_info* sem_info)
{
    if (semctl(sem_info->semid, 0, IPC_RMID, NULL) == -1)
    {
        perror("delete_sems");
        if (sem_info->sem_name)
            free(sem_info->sem_name);
        sem_info->sem_name = NULL;
        return -1;
    }
    if (sem_info->sem_name)
            free(sem_info->sem_name);
    sem_info->sem_name = NULL;
    return 0;
}

int capture_mutex(int semid)
{
    struct sembuf buf = {
        .sem_num = MUTEX_SEMNUM,
        .sem_flg = 0,
        .sem_op = -1
    };
    if (semop(semid, &buf, 1) == -1)
        ERROR("capture_mutex");
    return 0;
}

int release_mutex(int semid)
{
    struct sembuf buf = {
        .sem_num = MUTEX_SEMNUM,
        .sem_flg = 0,
        .sem_op = 1
    };
    if (semop(semid, &buf, 1) == -1)
        ERROR("release_mutex");
    return 0;
}

int sem_add_str(int semid)
{
    struct sembuf buf[2] = {
        {FULL_SEMNUM, 1, IPC_NOWAIT},
        {EMPTY_SEMNUM, -1, IPC_NOWAIT}
    };
    if (semop(semid, buf, 2) == -1)
        ERROR("sem_add_str");
    return 0;
}

int sem_del_str(int semid)
{
    struct sembuf buf[2] = {
        {FULL_SEMNUM, -1, IPC_NOWAIT},
        {EMPTY_SEMNUM, 1, IPC_NOWAIT}
    };
    if (semop(semid, buf, 2) == -1)
        ERROR("sem_del_str");
    return 0;
}

static int del_line_and_close(FILE* file, const char* filename)
{
    char tmp_name[FILENAME_MAX] = {};
    strcat(tmp_name, filename);
    strcat(tmp_name, ".tmp");

    FILE* tmp = fopen(tmp_name, "w");
    if (!tmp)
        ERROR("process_file (fclose)");
    char line[MAX_GENERATED_LENGTH];

    while (fgets(line, MAX_GENERATED_LENGTH, file))
        fputs(line, tmp);

    fclose(tmp);
    if (fclose(file) == -1)
        ERROR("process_file (fclose)");

    if (unlink(filename) != 0)
        ERROR("process_file (unlink)");
    if (rename(tmp_name, filename) != 0)
        ERROR("process_file (rename)");
    return 0;
}

int process_file(int semid, const char* filename)
{
    int min, max;
    FILE* file;
    char buffer[MAX_GENERATED_LENGTH];

    if (capture_mutex(semid) != 0)
        ERROR("process_file (capture_mutex)");
    if (sem_del_str(semid) != 0)
    {
        if (errno == EAGAIN)
        {
            printf("process_file: буфер %s пуст!\n", filename);
            release_mutex(semid);
            return 1;
        }
        release_mutex(semid);
        ERROR("process_file (sem_del_str)");
    }
    file = fopen(filename, "r+");
    if (!file)
        ERROR("process_file (fopen)");
    if (!fgets(buffer, MAX_GENERATED_LENGTH, file))
        ERROR("process_file (fgets)");

    // копирует все ниже первой (прочитанной выше)
    // строчки и перезаписывает файл
    if (del_line_and_close(file, filename) == -1)
    {
        sem_del_str(semid);
        release_mutex(semid);
        return -1;
    }

    if (release_mutex(semid) != 0)
        ERROR("process_file (release_mutex)");

    char* before = strdup(buffer);
    if (find_max_and_min(buffer, &max, &min) == -1)
        ERROR("process_file (find_max_and_min)");
    printf("Max %d, min %d, str: %s", max, min, before);
    fflush(stdout);

    return 0;
}

int append_file(int semid, const char* filename, const int min_num, const int max_num)
{
    FILE* file;

    // srand(time(NULL));
    char* buffer = gen_rand_num_str(min_num, max_num);

    printf("Generated string: %s\n", buffer);

    if (capture_mutex(semid) != 0)
        ERROR("append_file (capture_mutex)");
    if (sem_add_str(semid) != 0)
    {
        if (errno == EAGAIN)
        {
            printf("process_file: буфер %s полон!\n", filename);
            release_mutex(semid);
            return 1;
        }
        release_mutex(semid);
        ERROR("append_file (sem add str)");
    }
    file = fopen(filename, "a+");
    if (!file)
        ERROR("append_file (fopen)");
    if (fprintf(file, "%s\n", buffer) < 0)
        ERROR("append_file (fprintf)");
    if (fclose(file) == -1)
        ERROR("append_file (fclose)");
    if (release_mutex(semid) != 0)
        ERROR("append_file (release_mutex)");

    return 0;
}