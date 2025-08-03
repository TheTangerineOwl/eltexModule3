#include "chat.h"

run_info info;

void shutdown_handler(int sig)
{
    printf("%s: получен сигнал %d, завершение сеанса...\n", info.name, sig);

    fflush(stdout);

    if (close_server(&info) == -1)
        exit(EXIT_FAILURE);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    if (argc >= 2 && strcmp(argv[1], "1") == 0)
    {
        info.is_server = true;
        info.name = "Сервер";
        // mq_talk_name = Q_NAME_SERVER_TO_CLIENT;
        // mq_listen_name = Q_NAME_CLIENT_TO_SERVER;
    }
    else
    {
        info.is_server = false;
        info.name = "Клиент";
        // mq_talk_name = Q_NAME_SERVER_TO_CLIENT;
        // mq_listen_name = Q_NAME_CLIENT_TO_SERVER;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = shutdown_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    if (establish_connections(&info) != 0)
    {
        close_server(&info);
        return EXIT_FAILURE;
    }

    msg_t msg;
    msg.size = SIZE;
    msg_t* response = NULL;
    bool fail_exit = false;

    if (info.is_server)
    {
        printf("%s: жду клиента\n", info.name);
        fflush(stdout);
        response = listen(info.mq_listen);
        if (!response)
        {
            fail_exit = true;
        }
        else {
            if (response->priority == 0)
            {
                printf("%s: получено сообщение с 0 приоритетом (shutdown).\n"
                    "%s: выполняется отключение...\n", info.name, info.name);
            }
            else {
            printf("%s: принято сообщение с приоритетом %u и длиной %lu,"
                " текст: %s\n", info.name, response->priority, response->size, response->body);
            }
            free(response);
            response = NULL;
        }
    }
    while (!fail_exit)
    {
        printf("%s: введите сообщение: ", info.name);
        fflush(stdout);

        if (fgets(msg.body, SIZE, stdin) == NULL)
        {
            fail_exit = true;
            break;
        }
        msg.body[strcspn(msg.body, "\n")] = '\0';
        
        if (strlen(msg.body) == 0)
            continue;
        if (strcmp(msg.body, "shutdown") == 0)
            msg.priority = 0;
        else
            msg.priority = 1;

        // printf("%s: отправка сообщения с приоритетом %u и длиной %lu,"
            //    " текст: %s\n", info.name, msg.priority, msg.size, msg.body);
        // fflush(stdout);
        if (talk(info.mq_talk, msg) == -1)
        {
            fail_exit = true;
            break;
        }

        if (msg.priority == 0)
            break;

        printf("%s: жду ответ\n", info.name);
        fflush(stdout);
        response = listen(info.mq_listen);
        if (!response)
        {
            fail_exit = true;
            break;
        }

        if (response->priority == 0)
        {
            printf("%s: получено сообщение с 0 приоритетом (shutdown).\n"
                   "%s: выполняется отключение...\n", info.name, info.name);
            break;
        }

        printf("%s: принято сообщение с приоритетом %u и длиной %lu,"
               " текст: %s\n", info.name, response->priority, response->size, response->body);

        free(response);
        response = NULL;
    }

    if (response)
        free(response);
    response = NULL;
    if (close_server(&info) == -1 || fail_exit)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}