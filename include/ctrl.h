//
// Created by alvis on 2021/8/7.
//

#ifndef LFTP_CTRL_CTRL_H
#define LFTP_CTRL_CTRL_H


int ctrl_conn_create(int port);
int ctrl_conn_close();
int ctrl_send_file(const char *filename, const char *ip, char *s_port);
int ctrl_set_conf(const char *conf, const char *value);


#define CTRL_ERR_NOT_CONN (-1)
#define CTRL_ERR_NULL_PTR (-2)
#define CTRL_ERR_SEND_FAILED (-3)
#define CTRL_ERR_RECV_FAILED (-4)
#define CTRL_ERR_OPERATE_FAILED (-5)


#endif //LFTP_CTRL_CTRL_H
