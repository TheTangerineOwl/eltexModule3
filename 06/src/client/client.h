#include "../chat.h"

// extern int client_id;

int connect_queue();
int send_msg(int qid, msg_t* msg);
int receive_msg(int qid, const int my_id, msg_t* msg);
