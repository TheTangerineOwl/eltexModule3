#include "chat.h"

void init_socket(int port)
{
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
    {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

void *receive_messages(void *arg)
{
    char buffer[BUFFER_SIZE];
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    
    while (1)
    {
        int len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                         (struct sockaddr *)&from_addr, &addr_len);
        if (len == -1)
        {
            perror("receive_messages");
            return NULL;
        }
        else if (len > 0)
        {
            buffer[len] = '\0';
            
            if (is_server && !peer_known)
            {
                peer_addr = from_addr;
                peer_known = true;
                printf("\nPeer connected from %s:%d\n",
                       inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port));
            }
            
            printf("\nPeer: %s\n", buffer);
            printf("You: ");
            fflush(stdout);
        }
    }
    return NULL;
}
