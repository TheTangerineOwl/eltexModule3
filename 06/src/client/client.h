#include "../chat.h"

int connect_queue();
int send_msg(int qid, msg_t* msg);
int receive_msg(int qid, const int my_id, msg_t* msg);
int prepare_shutdown(int queue, long my_id);
