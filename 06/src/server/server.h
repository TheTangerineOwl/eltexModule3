#include "../chat.h"

extern int client_count;
extern int client_list[MAX_CLIENTS];

int create_queue();
int close_queue(const int qid);
int get_client(const int client_id);
int add_client_id(const int client_id);
int remove_client_id(const int client_id);
int get_msg(const int qid, msg_t* msg);
// int send_ack(const int qid, const int client);
int process_msg(msg_t* msg);
int redirect_msg(const int qid, msg_t* msg);
