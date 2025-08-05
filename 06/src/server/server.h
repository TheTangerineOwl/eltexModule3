#include "../chat.h"

extern int client_count;
extern long client_list[MAX_CLIENTS];

int create_queue();
int close_queue(const int qid);
int get_client(const int client_id);
int add_client_id(const int client_id);
int remove_client_id(const long client_id);
int get_msg(const int qid, msg_t* msg);
int send_ack(const int qid, const long client);
int send_bad(const int qid, const long client);
int process_msg(const int qid, msg_t* msg);
int redirect_msg(const int qid, msg_t* msg);
