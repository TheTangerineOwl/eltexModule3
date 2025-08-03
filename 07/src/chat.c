#include "chat.h"

int establish_connections(run_info* info)
{
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = MAX_MSG,
        .mq_msgsize = SIZE,
        .mq_curmsgs = 0
    };
    fflush(stdout);
    char* talker = info->is_server ? Q_NAME_SERVER_TO_CLIENT : Q_NAME_CLIENT_TO_SERVER;
    info->mq_talk = open_connection(talker, &attr, false);
    if (info->mq_talk == (mqd_t)-1)
        return -1;
    char* listener = !info->is_server ? Q_NAME_SERVER_TO_CLIENT : Q_NAME_CLIENT_TO_SERVER;
    info->mq_listen = open_connection(listener, &attr, true);
    if (info->mq_listen == (mqd_t)-1)
        return -1;
    return 0;
}

mqd_t open_connection(const char* name, struct mq_attr* attr, bool is_listener)
{
    int flags = is_listener ? O_CREAT | O_RDONLY : O_CREAT | O_WRONLY;
    mqd_t mq = mq_open(name, flags, FILEMODE, attr);
    if (mq == (mqd_t)-1)
    {
        perror("Не удалось создать соединение");
        return -1;
    }
    return mq;
}

int close_connection(mqd_t* mq)
{
    if (mq_close(*mq) == -1)
    {
        perror("не удалось разорвать соединение");
        return -1;
    }
    return 0;
}

int close_server(run_info* info)
{
    if (close_connection(&info->mq_talk) == -1)
        return -1;
    char* talker_name = info->is_server ? Q_NAME_SERVER_TO_CLIENT : Q_NAME_CLIENT_TO_SERVER;
    if (mq_unlink(talker_name) == -1)
    {
        perror("Сервер: не удалось удалить очередь");
        return -1;
    }
    return 0;
}

int talk(const mqd_t listener, const msg_t msg)
{
    int sent = mq_send(listener, msg.body, msg.size, msg.priority);
    if (sent == -1)
    {
        perror("Не удалось отправить сообщение!");
        return -1;
    }
    return 0;
}

msg_t* listen(const mqd_t talker)
{
    msg_t* msg = (msg_t*)malloc(sizeof(msg_t));
    msg->size = SIZE;
    ssize_t received = mq_receive(talker, msg->body, msg->size, &msg->priority);
    if (received == -1)
    {
        perror("Не удалось получить сообщение!");
        return NULL;
    }
    return msg;
}