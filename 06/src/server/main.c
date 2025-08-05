#include "server.h"

int client_count;
long client_list[MAX_CLIENTS];

int main()
{
    int queue = create_queue();
    if (queue == -1)
        return EXIT_FAILURE;
    client_count = 0;
    msg_t msg;
    puts("Сервер запущен");
    fflush(stdout);
    while (1)
    {
        int received = get_msg(queue, &msg);
        if (received == -1)
            continue;

        process_msg(queue, &msg);

        if (redirect_msg(queue, &msg) != 0)
        {
            printf("Сервер: не удалось отправить сообщение:\n"
                   "\t- Тип: %ld\n"
                   "\t- Отправитель: %ld\n"
                   "\t- Получатель: %ld\n"
                   "\t- Текст: %s\n", msg.mtype, msg.sender, msg.receiver, msg.mtext);
            send_bad(queue, msg.sender);
        }
        else
        {
            printf("Сервер: получено и перенаправлено сообщение:\n"
                   "\t- Тип: %ld\n"
                   "\t- Отправитель: %ld\n"
                   "\t- Получатель: %ld\n"
                   "\t- Текст: %s\n", msg.mtype, msg.sender, msg.receiver, msg.mtext);
        }

        printf("Активные пользователи:\n");
        for (int i = 0; i < client_count; i++)
            printf("%ld ", client_list[i]);
        puts("\nСервер ждет сообщений...");
        fflush(stdout);
    }

    if (close_queue(queue) == -1)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}