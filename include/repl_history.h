//
// Created by alvis on 2021/8/7.
//

#ifndef LFTP_CTRL_REPL_HISTORY_H
#define LFTP_CTRL_REPL_HISTORY_H


#define HIST_ERR_INVALID_HEAD (-1)
#define HIST_ERR_INVALID_NODE (-2)
#define HIST_ERR_NO_MEMORY (-3)


typedef struct hist_list * hist_list_t;

int hist_errno_get();

hist_list_t hist_list_create();
int hist_list_destroy(hist_list_t head);
hist_list_t hist_node_create(char *cmd_line, int is_copy);
int hist_node_add(hist_list_t head, hist_list_t node);
int hist_cmd_add(hist_list_t head, char *cmd_line, int is_copy);
int hist_node_del(hist_list_t node);
int hist_node_clear(hist_list_t head);



#endif //LFTP_CTRL_REPL_HISTORY_H
