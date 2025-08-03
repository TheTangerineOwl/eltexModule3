#include "client.h"

int main(int argc, char* argv[])
{
    long client_id;
    if (argc < 2)
    {
        puts("Клиент: не указан id!");
        return EXIT_FAILURE;
    }
    client_id = atol(argv[1]);
    if (client_id == 0)
    {
        perror("Клиент: не удалось получить id");
        return EXIT_FAILURE;
    }
    else if (client_id == SERVER_ID)
    {
        puts("Клиент: id не может совпадать с id сервера!");
        return EXIT_FAILURE;
    }

    int queue = connect_queue();
    if (queue == -1)
        return EXIT_FAILURE;

    long receiver;
    msg_t msg = {
        .mtype = SERVER_ID,
        .sender = client_id,
        .receiver = SERVER_ID,
        .mtext = "HELLO"
    };

    // сообщаем серверу о клиенте
    int sent = send_msg(queue, &msg);
    if (sent != 0)
        return EXIT_FAILURE;
    // получили ответ от сервера
    int received = msgrcv(queue, &msg, MSG_SIZE, client_id, 0);
    if (received == -1 || msg.sender != SERVER_ID || strcmp(msg.mtext, "ACK") != 0)
        return EXIT_FAILURE;

    char c;

    while (1)
    {
        msg.mtype = SERVER_ID;
        msg.sender = client_id;

        printf("Клиент %ld: введите сообщение: ", client_id);
        fflush(stdout);

        if (fgets(msg.mtext, MSG_TEXT_SIZE, stdin) == NULL)
            break;

        msg.mtext[strcspn(msg.mtext, "\n")] = '\0';
        
        if (strlen(msg.mtext) != 0)
        {
            if (strcmp(msg.mtext, "shutdown") == 0)
            {
                msg.receiver = SERVER_ID;

                sent = send_msg(queue, &msg);

                printf("Клиент %ld: жду подтверждение отключения...\n", client_id);
                fflush(stdout);

                if (prepare_shutdown(queue, client_id) == -1)
                    return EXIT_FAILURE;

                break;
            }

            printf("Клиент %ld: введите id получателя: ", client_id);
            fflush(stdout);

            while (!scanf("%ld", &receiver) || receiver < 1)
            {
                puts("Некорректный выбор! Попробуйте еще раз.\n");
                while ((c = getchar() != '\n') && c != EOF);
            }

            while ((c = getchar()) != '\n' && c != EOF);
            msg.receiver = receiver;
            
            sent = send_msg(queue, &msg);
            if (sent == 0)
                printf("Клиент %ld: отправлено сообщение:\n"
                   "\t- Адресат: %ld\n"
                   "\t- Текст: %s\n", client_id, msg.receiver, msg.mtext);
        }

        received = 0;
        while (received != 1)
        {
            msg.mtext[0] = '\0';
            msg.mtype = 0;
            msg.receiver = 0;
            msg.sender = 0;

            received = receive_msg(queue, client_id, &msg);
            if (received == -1)
                break;
            if (msg.receiver == client_id)
                printf("Клиент %ld: принято сообщение:\n"
                    "\t- Отправитель: %ld\n"
                    "\t- Текст: %s\n", client_id, msg.sender, msg.mtext);
        }
        puts("Клиент: нет новых сообщений");
    }
    
    return EXIT_SUCCESS;
}