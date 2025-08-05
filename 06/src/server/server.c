#include "server.h"

extern int client_count;
extern long client_list[MAX_CLIENTS];

int create_queue()
{
    key_t key = GET_KEY();
    int queue = msgget(key, IPC_CREAT | FILEMODE);
    if (queue == -1)
    {
        perror("Сервер: не удалось создать очередь сообщений!");
        return -1;
    }
    return queue;
}

int close_queue(const int qid)
{
    if (msgctl(qid, IPC_RMID, NULL) == -1)
    {
        perror("Сервер: не удалось удалить очередь сообщений!");
        return -1;
    }
    return 0;
}

int get_client(const int client_id)
{
    int active = -1;
    for (int i = 0; i < client_count && i < MAX_CLIENTS; i++)
        if (client_list[i] == client_id)
            active = i;
    return active;
}

int add_client_id(const int client_id)
{
    if (get_client(client_id) != -1)
    {
        perror("Сервер: клиент с таким id уже подключен!");
        return -1;
    }
    if (client_count + 1 > MAX_CLIENTS)
    {
        perror("Сервер: нельзя добавить еще одно подключение!");
        return -1;
    }
    client_list[client_count] = client_id;
    client_count++;
    return client_id;
}

int remove_client_id(const long client_id)
{
    int client_index = get_client(client_id);

    if (client_index == -1)
    {
        perror("Сервер: клиент с таким id отсутствует!");
        return -1;
    }
    for (int i = client_index; i < client_count && i < MAX_CLIENTS - 1; i++)
        client_list[i] = client_list[i + 1];
    client_list[MAX_CLIENTS - 1] = 0;
    client_count--;
    return client_id;
}

int get_msg(const int qid, msg_t* msg)
{
    int bytes = msgrcv(qid, msg, MSG_SIZE, SERVER_ID, MSG_NOERROR);
    if (errno == ENOMSG)
        return 1;
    else if (bytes == -1)
    {
        perror("Сервер: ошибка чтения сообщения!");
        return -1;
    }
    return 0;
}

int send_ack(const int qid, const long client)
{
    int receiver = get_client(client);
    if (receiver == -1)
        return -2;
    msg_t msg = {
        .mtype = client,
        .receiver = client,
        .sender = SERVER_ID,
        .mtext = "ACK"
    };
    int send = msgsnd(qid, &msg, MSG_SIZE, IPC_NOWAIT);
    if (send == -1)
    {
        perror("Сервер: ошибка отправки сообщения!");
        return -1;
    }
    return 0;
}

int send_bad(const int qid, const long client)
{
    int receiver = get_client(client);
    if (receiver == -1)
        return -2;
    msg_t msg = {
        .mtype = client,
        .receiver = client,
        .sender = SERVER_ID,
        .mtext = "BAD"
    };
    int send = msgsnd(qid, &msg, MSG_SIZE, IPC_NOWAIT);
    if (send == -1)
    {
        perror("Сервер: ошибка отправки сообщения!");
        return -1;
    }
    return 0;
}

int process_msg(const int qid, msg_t* msg)
{
    int cid = msg->sender;
    if (get_client(cid) == -1)
    {
        if (add_client_id(cid) != -1)
        {
            printf("Сервер: добавлен клиент %d\n", cid);
            fflush(stdout);
            send_ack(qid, cid);
        }
    }
    if (get_client(msg->receiver) == -1 && msg->receiver != SERVER_ID)
    {
        printf("Сервер: клиент %ld не найден!\n", msg->receiver);
        fflush(stdout);
        send_bad(qid, msg->receiver);
    }
    if (strcmp(msg->mtext, "shutdown") == 0)
    {
        printf("Сервер: получен сигнал на отключение от клиента %d\n", cid);
        fflush(stdout);
        send_ack(qid, cid);
        remove_client_id(cid);
        return 0;
    }
    return 0;
}

int redirect_msg(const int qid, msg_t* msg)
{
    if (msg->receiver == SERVER_ID)
        return 0;
    int receiver = get_client(msg->receiver);
    if (receiver == -1)
    {
        perror("Сервер: клиент с таким id отсутствует!");
        return -2;
    }
    msg->mtype = msg->receiver;
    int send = msgsnd(qid, msg, MSG_SIZE, 0);
    if (send == -1)
    {
        perror("Сервер: ошибка перенаправления сообщения!");
        return -1;
    }
    else
    {
        printf("Сервер: перенаправляю сообщение от %ld для %ld\n", msg->sender, msg->receiver);
        fflush(stdout);
    }
    return 0;
}