//
// Created by alvis on 2021/8/7.
//

#ifndef LFTP_CTRL_REPL_LOOP_H
#define LFTP_CTRL_REPL_LOOP_H


#define CLI_NAME "LFTP"
#define CLI_BREAKER "# "
#define CMD_LINE_MAX 1024

void main_loop(const char* ip, int port);
void main_loop_raw(const char *ip, int port);

#endif //LFTP_CTRL_REPL_LOOP_H
