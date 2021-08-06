//
// Created by alvis on 2021/8/7.
//

#ifndef LFTP_CTRL_CTRL_H
#define LFTP_CTRL_CTRL_H


int ctrl_conn_create(int port);
int ctrl_conn_close();
int ctrl_send_file(const char *filename, const char *ip, char *s_port);



#endif //LFTP_CTRL_CTRL_H
