#include "../chat.h"

mqd_t mq_talk;
mqd_t mq_listen;

void shutdown_handler(int sig)
{
    printf("Клиент: получен сигнал %d, завершение сеанса...\n", sig);
    fflush(stdout);

    if (close_server(&mq_talk, Q_NAME_CLIENT_TO_SERVER) == -1)
        exit(EXIT_FAILURE);
    exit(EXIT_SUCCESS);
}

int main()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = shutdown_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    if (establish_connections(Q_NAME_CLIENT_TO_SERVER, Q_NAME_SERVER_TO_CLIENT) != 0)
    {
        close_server(&mq_talk, Q_NAME_CLIENT_TO_SERVER);
        return EXIT_FAILURE;
    }

    msg_t msg;
    msg.size = SIZE;
    msg_t* response = NULL;
    bool fail_exit = false;
    while (!fail_exit)
    {
        printf("Клиент: введите сообщение: ");
        fflush(stdout);

        if (fgets(msg.body, SIZE, stdin) == NULL)
        {
            fail_exit = true;
            break;
        }
        msg.body[strcspn(msg.body, "\n")] = '\0';
        
        if (strcmp(msg.body, "shutdown") == 0)
            msg.priority = 0;
        else
            msg.priority = 2;

        printf("Клиент: отправка сообщения с приоритетом %u и длиной %lu,"
               " текст: %s\n", msg.priority, msg.size, msg.body);
        fflush(stdout);

        if (talk(mq_talk, msg) == -1)
        {
            fail_exit = true;
            break;
        }

        if (msg.priority == 0)
            break;

        response = listen(mq_listen);
        if (!response)
        {
            fail_exit = true;
            break;
        }

        if (response->priority == 0)
        {
            printf("Клиент: получено сообщение с 0 приоритетом (shutdown).\n"
                   "Клиент: выполняется отключение...\n");
            break;
        }

        printf("Клиент: принято сообщение с приоритетом %u и длиной %lu,"
               " текст: %s\n", response->priority, response->size, response->body);

        free(response);
        response = NULL;
    }

    if (response)
        free(response);
    response = NULL;
    if (close_server(&mq_talk, Q_NAME_CLIENT_TO_SERVER) == -1 || fail_exit)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}