#include "server.h"

int client_count;
int client_list[MAX_CLIENTS];

int main()
{
    int queue = create_queue();
    if (queue == -1)
        return EXIT_FAILURE;
    client_count = 0;
    msg_t msg;
    while (1)
    {
        int received = get_msg(queue, &msg);
        if (received == -1)
            continue;
        else if (received == 1)
        {
            // puts("Сервер: нет сообщений");
            continue;
        }
        
        process_msg(&msg);

        if (redirect_msg(queue, &msg) != 0)
        {
            printf("Сервер: не удалось отправить сообщение:\n"
                   "\t- Отправитель: %ld\n"
                   "\t- Получатель: %ld\n"
                   "\t- Текст: %s\n", msg.sender, msg.receiver, msg.mtext);
        }
        msg.mtext[0] = '\0';
        msg.mtype = SERVER_ID;
        msg.receiver = SERVER_ID;
        msg.sender = SERVER_ID;
    }

    if (close_queue(queue) == -1)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}