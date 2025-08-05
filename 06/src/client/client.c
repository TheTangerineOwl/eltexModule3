#include "client.h"

int connect_queue()
{
    key_t key = GET_KEY();
    int queue = msgget(key, FILEMODE);
    if (queue == -1)
    {
        perror("Клиент: не удалось получить очередь сообщений!");
        return -1;
    }
    return queue;
}

int send_msg(int qid, msg_t* msg)
{
    msg->mtype = SERVER_ID;
    int send = msgsnd(qid, msg, MSG_SIZE, 0);
    if (send == -1)
    {
        perror("Клиент: ошибка отправки сообщения!");
        return -1;
    }
    return 0;
}

int receive_msg(int qid, const int my_id, msg_t* msg)
{
    int bytes = msgrcv(qid, msg, MSG_SIZE, my_id, IPC_NOWAIT | MSG_NOERROR);
    if (errno == ENOMSG)
    {
        // puts("Клиент: нет новых сообщений");
        return 1;
    }
    else if (bytes == -1)
    {
        perror("Клиент: ошибка чтения сообщения!");
        return -1;
    }
    return 0;
}

int prepare_shutdown(int queue, long my_id)
{
    int received = 0;
    msg_t msg = {
        .mtype = 0,
        .sender = 0,
        .receiver = 0,
        .mtext = ""
    };
    while (received != -1)
    {
        received = receive_msg(queue, my_id, &msg);
        if (received == -1)
            return -1;
        // else if (received == 0)
        // {
            if (msg.sender == SERVER_ID && strcmp(msg.mtext, "ACK") == 0)
            {
                printf("Клиент %ld: подтверждение на отключение получено.\n", my_id);
                break;
            }
            else if (msg.receiver == my_id)
            {
                printf("Клиент %ld: принято сообщение:\n"
                    "\t- Отправитель: %ld\n"
                    "\t- Текст: %s\n",
                    my_id, msg.sender, msg.mtext);
                msg.receiver = 0;
            }
        // }
    }
    return 0;
}