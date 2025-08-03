#include "client.h"

// int client_id;

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
        puts("Клиент: некорректный id!");
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
        .mtext = ""
    };

    int sent = send_msg(queue, &msg);
    if (sent != 0)
        return EXIT_FAILURE;

    char c;

    while (1)
    {
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
                break;
            }

            printf("Клиент %ld: введите id получателя: ", client_id);
            fflush(stdout);

            while (!scanf("%ld", &receiver) || receiver < 1)
            {
                puts("Некорректный выбор! Попробуйте еще раз.\n");
                while ((c = getchar() != '\n') && c != EOF);
            }
            msg.receiver = receiver;
            msg.mtype = SERVER_ID;
            while ((c = getchar()) != '\n' && c != EOF);
            
            sent = send_msg(queue, &msg);
            if (!sent)
                printf("Отправлено клиенту %ld!\n", receiver);
        }

        msg.mtext[0] = '\0';
        msg.mtype = client_id;
        msg.receiver = client_id;
        msg.sender = client_id;
        int received = receive_msg(queue, client_id, &msg);
        if (received == 1)
            continue;
        else if (received != -1)
        {
            printf("Клиент %ld: принято сообщение:\n"
                   "\t- Отправитель: %ld\n"
                   "\t- Текст: %s\n", client_id, msg.sender, msg.mtext);
        }
    }
    
    return EXIT_SUCCESS;
}