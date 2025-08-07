// #define _DEFAULT_SOURCE
// #define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
 
// количество активных пользователей
int nclients;
int sockfd, newsockfd;

 // функция обработки ошибок 
 void error(const char *msg)
 {
    perror(msg);
    exit(1);
}

// печать количества активных
// пользователей 
void printusers()
{ 
    if(nclients)
    {
        printf("%d user on-line\n", nclients);
    }
    else
    {
        printf("No User on line\n");
    }
}

// функция обработки данных
int myfunc(char op, int* res, int a, int b)
{
    switch (op)
    {
        case '+': *res = a + b; break;
        case '-': *res = a - b; break;
        case '*': *res = a * b; break;
        case '/':
        {
            if (b == 0)
                return -1;
            *res = a / b;
            break;
        }
        default:
            return -1;
    }
    return 0;
} 

 // функция обслуживания
 // подключившихся пользователей
 void dostuff(int sock)
{
    int bytes_recv; // размер принятого сообщения
    int a,b; // переменные для myfunc
    #define bufsize 20 * 1024
    char buff[bufsize];
    #define str1 "Enter 1 parameter\r\n"
    #define str2 "Enter 2 parameter\r\n"
    #define str3 "Choose an operation (+, -, *, /)\r\n"
    // отправляем клиенту сообщение
    write(sock, str1, sizeof(str1));
    // обработка первого параметра
    bytes_recv = read(sock,&buff[0],sizeof(buff));
    if (bytes_recv < 0)
        error("ERROR reading from socket");
    a = atoi(buff); // преобразование первого параметра в int
    // отправляем клиенту сообщение
    write(sock,str2,sizeof(str2));
    bytes_recv = read(sock,&buff[0],sizeof(buff));
    if (bytes_recv < 0)
        error("ERROR reading from socket");
    b = atoi(buff); // преобразование второго параметра в int
    write(sock,str3,sizeof(str3));
    bytes_recv = read(sock,&buff[0],sizeof(buff));
    if (bytes_recv < 0)
        error("ERROR reading from socket");
    char op = buff[strcspn(buff, "+-*/")];
    if (myfunc(op, &a, a, b) == -1)
    {
        if (b == 0)
            snprintf(buff, bufsize, "Invalid argument: can't divide by 0!");
        else
            snprintf(buff, bufsize, "Invalid operation!");
    }
    else
    {
        // a = myfunc(a,b); // вызов пользовательской функции
        snprintf(buff, bufsize, "%d", a); // преобразование результата в строку
    }
    buff[strlen(buff)] = '\n'; // добавление к сообщению символа конца строки
    // отправляем клиенту результат
    write(sock,&buff[0], sizeof(buff));
    // (*nclients)--; // уменьшаем счетчик активных клиентов
    printf("-disconnect\n"); 
    // printusers();
    return;
} 

void cleanup(void)
{
    close(newsockfd);
    close(sockfd);
    free(nclients);
}

void signal_handler(int sig)
{
    printf("\nExiting...\n");
    cleanup();
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);

    // char buff[1024]; // Буфер для различных нужд
    // int sockfd, newsockfd; // дескрипторы сокетов
    int portno; // номер порта
    int pid; // id номер потока
    socklen_t clilen; // размер адреса клиента типа socklen_t
    struct sockaddr_in serv_addr, cli_addr; // структура сокета сервера и клиента
    printf("TCP SERVER DEMO\n");
    
    // ошибка в случае если мы не указали порт
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    } 

    // Шаг 1 - создание сокета
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    // Шаг 2 - связывание сокета с локальным адресом
    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    nclients = 0;
    // Шаг 3 - ожидание подключений, размер очереди - 5
    listen(sockfd, 5);
    clilen = sizeof(cli_addr); 

    // Шаг 4 - извлекаем сообщение из очереди (цикл извлечения запросов на подключение)
    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        nclients++;
        
        // вывод сведений о клиенте
        struct hostent *hst;
        hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s] new connect!\n", (hst) ? hst->h_name : "Unknown host", (char*)inet_ntoa(cli_addr.sin_addr));
        printusers();
        fflush(stdout);
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)
        {
            close(sockfd);
            dostuff(newsockfd);
            nclients--;
            printusers();
            fflush(stdout);
            exit(0);
        }
        else
        {
            // atexit(cleanup);
            close(newsockfd);
        }
    }
    cleanup();
    // free(nclients);
    // close(sockfd);
    return 0;
}
