// #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>

#define BUFFER_SIZE 65536
#define DUMP_FILE "packet_dump.bin"

// Функция для дампа пакета в файл
void dump_packet(const unsigned char *packet, int length)
{
    FILE *fp = fopen(DUMP_FILE, "ab");
    if (fp == NULL)
    {
        perror("fopen");
        return;
    }
    
    fwrite(packet, 1, length, fp);
    fclose(fp);
}

// Функция для извлечения и вывода UDP данных
void process_udp_packet(const unsigned char *buffer)//, int size)
{
    struct iphdr *iph = (struct iphdr*)buffer;
    unsigned int iphdrlen = iph->ihl*4; // ihl - количество 32-битных СЛОВ в заголовке (могут быть опции)
    char source[16];
    inet_ntop(AF_INET, &iph->saddr, source, 16);
    printf("IP-source: %s\n", source);
    char destination[16];
    inet_ntop(AF_INET, &iph->daddr, destination, 16);
    printf("IP-destination: %s\n", destination);
    u_int8_t protocol = iph->protocol;
    printf("Protocol: %hhu\n", protocol);

    struct udphdr *udph = (struct udphdr*)(buffer + iphdrlen);
    printf("Source port: %hu\n", ntohs(udph->uh_sport));
    printf("Destination port: %hu\n", ntohs(udph->uh_dport));
    printf("Length (with UDP header): %hu\n", ntohs(udph->len));

    int udp_data_len = ntohs(udph->len) - sizeof(struct udphdr);
    unsigned char *udp_data = (unsigned char*)buffer + iphdrlen + sizeof(struct udphdr);


    // Проверяем, что данные есть
    if (udp_data_len > 0)
    {
        printf("\nUDP Message: ");
        // Выводим как строку (если это текст)
        for(int i = 0; i < udp_data_len; i++)
        {
            if (udp_data[i] >= 32 && udp_data[i] <= 126)
            {
                printf("%c", udp_data[i]);
            }
            else
            {
                printf("\\x%02x", udp_data[i]);
            }
        }
        printf("\n");
        
        // Выводим hex dump
        printf("Hex dump (%d bytes):\n", udp_data_len);
        for(int i = 0; i < udp_data_len; i++)
        {
            printf("%02x ", udp_data[i]);
            if ((i+1) % 8 == 0)
                printf("  "); 
            if ((i+1) % 16 == 0)
                printf("\n");
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    int raw_socket;
    unsigned char buffer[BUFFER_SIZE];
    
    if (argc != 2)
    {
        printf("Usage: %s <port_to_sniff>\n", argv[0]);
        exit(1);
    }
    
    int port_to_sniff = atoi(argv[1]);
    
    // Создаем RAW сокет для захвата IP пакетов
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (raw_socket < 0)
    {
        perror("socket");
        exit(1);
    }
    
    printf("Starting sniffer on port %d...\n", port_to_sniff);
    printf("Press Ctrl+C to stop and view dump.\n");
    printf("Dump will be saved to %s\n", DUMP_FILE);
    
    // Удаляем старый файл дампа, если он существует
    remove(DUMP_FILE);
    
    while(1)
    {
        int packet_size = recvfrom(raw_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (packet_size < 0)
        {
            perror("recvfrom");
            close(raw_socket);
            exit(1);
        }
        
        struct iphdr *iph = (struct iphdr*)buffer;
        if (iph->protocol == IPPROTO_UDP)
        {
            struct udphdr *udph = (struct udphdr*)(buffer + (iph->ihl*4));
            
            // Проверяем, что пакет предназначен для нужного порта
            if (ntohs(udph->dest) == port_to_sniff)
            {
                // Сохраняем пакет в дамп
                dump_packet(buffer, packet_size);
                
                // Обрабатываем UDP данные
                process_udp_packet(buffer);//, packet_size);
            }
        }
    }
    
    close(raw_socket);
    return 0;
}