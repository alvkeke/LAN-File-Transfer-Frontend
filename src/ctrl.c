//
// Created by alvis on 2021/8/7.
//

#include "ctrl.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>



static int g_ctrl_conn_fd = -1;


int ctrl_conn_create(int port)
{

    if (g_ctrl_conn_fd >= 0) return 0;
    // if connection was created before, return directly.

    g_ctrl_conn_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (g_ctrl_conn_fd < 0) return errno;

    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(port);
    ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(g_ctrl_conn_fd, (const struct sockaddr *) &ser_addr, sizeof(ser_addr)))
    {
        int err_save = errno;
        close(g_ctrl_conn_fd);
        return err_save;
    }

    return 0;
}


static char str_ret_success[] = "success\n";
static char str_ret_failed[256];

int ctrl_send_file(const char *filename, const char *ip, char *s_port)
{
    if (!filename || !ip)
    {
        return -1;
    }

    size_t filename_len = strlen(filename);
    size_t ip_len = strlen(ip);
    size_t port_len = strlen(s_port);
    size_t data_len = filename_len + ip_len + port_len + strlen("send   \n");

    char* data_buf = (char*) malloc(data_len);

    char *cp_pt = data_buf;
    static char send_cmd_head[] = "send ";
    strcpy(cp_pt, send_cmd_head);
    cp_pt += strlen(send_cmd_head);
    strcpy(cp_pt, filename);
    cp_pt += filename_len;
    *(cp_pt++) = ' ';
    strcpy(cp_pt, ip);
    cp_pt += ip_len;
    *(cp_pt++) = ' ';
    strcpy(cp_pt, s_port);
    cp_pt += port_len;

    *(cp_pt++) = '\n';
    *(cp_pt++) = '\0';

    ssize_t trans_lan = send(g_ctrl_conn_fd, data_buf, data_len, 0);
    if (trans_lan < 0)
    {
        free(data_buf);
        return -2;
    }

    memset(data_buf, 0, data_len);

    trans_lan = recv(g_ctrl_conn_fd, data_buf, data_len, 0);
    if (trans_lan < 0)
    {
        free(data_buf);
        return -3;
    }

    if (strcmp(data_buf, str_ret_success) == 0)
    {
        free(data_buf);
        return 0;
    }

    memcpy(str_ret_failed, data_buf, trans_lan);
    free(data_buf);
    return -4;
}



int ctrl_conn_close()
{
    static char exit_cmd[] = "exit\n";
    send(g_ctrl_conn_fd, exit_cmd, sizeof(exit_cmd), 0);
    return close(g_ctrl_conn_fd) ? errno : 0;
}


