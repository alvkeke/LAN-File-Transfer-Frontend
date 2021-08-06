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

/**
 * create a connection to LFTP server, this function will not return a socket fd for user
 * to operate the network. the socket fd will save in a global scope.
 * this method will return success directly if connection was opened before, unless the
 * method ctrl_conn_close() was called.
 * @param port the port to the LFTP ctrl server.
 * @return 0 if success, otherwise errno
 */
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

/**
 * send a request to LFTP server for sending a file.
 * @param filename file need to be send, string
 * @param ip IP address of target device, string
 * @param s_port port of target device, string
 * @return 0 if success, otherwise CTRL_ERRNO.
 */
int ctrl_send_file(const char *filename, const char *ip, char *s_port)
{
    if (g_ctrl_conn_fd < 0) return CTRL_ERR_NOT_CONN;
    if (!filename || !ip || !s_port) return CTRL_ERR_NULL_PTR;


    size_t filename_len = strlen(filename);
    size_t ip_len = strlen(ip);
    size_t port_len = strlen(s_port);
    size_t buf_len = filename_len + ip_len + port_len + strlen("send   \n");

    char* data_buf = (char*) malloc(buf_len);

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

    ssize_t trans_len = send(g_ctrl_conn_fd, data_buf, buf_len, 0);
    if (trans_len < 0)
    {
        free(data_buf);
        return CTRL_ERR_SEND_FAILED;
    }

    memset(data_buf, 0, buf_len);

    trans_len = recv(g_ctrl_conn_fd, data_buf, buf_len, 0);
    if (trans_len < 0)
    {
        free(data_buf);
        return CTRL_ERR_RECV_FAILED;
    }

    if (strcmp(data_buf, str_ret_success) == 0)
    {
        free(data_buf);
        return 0;
    }

    memcpy(str_ret_failed, data_buf, trans_len);
    free(data_buf);
    return CTRL_ERR_OPERATE_FAILED;
}


/**
 * send a request to LFTP server for configure settings.
 * @param conf name of configure item want to change
 * @param value new value of the item
 * @return 0 if success, otherwise CTRL_ERRNO
 */
int ctrl_set_conf(const char *conf, const char *value)
{
    if (g_ctrl_conn_fd<0) return CTRL_ERR_NOT_CONN;
    if (!conf || !value) return CTRL_ERR_NULL_PTR;

    size_t conf_len = strlen(conf);
    size_t value_len = strlen(value);

    size_t buf_len = conf_len + value_len + strlen("set  \n");
    char *data_buf = (char*) malloc(buf_len);

    char *cp_pt = data_buf;
    static char conf_cmd_head[] = "set ";
    strcpy(cp_pt, conf_cmd_head);
    cp_pt += strlen(conf_cmd_head);
    strcpy(cp_pt, conf);
    cp_pt += conf_len;
    *(cp_pt++) = ' ';
    strcpy(cp_pt, value);
    cp_pt += value_len;
    *(cp_pt++) = '\n';
    *(cp_pt++) = 0x00;

    ssize_t trans_len = send(g_ctrl_conn_fd, data_buf, buf_len, 0);
    if (trans_len < 0)
    {
        free(data_buf);
        return CTRL_ERR_SEND_FAILED;
    }

    trans_len = recv(g_ctrl_conn_fd, data_buf, buf_len, 0);
    if (trans_len < 0)
    {
        free(data_buf);
        return CTRL_ERR_RECV_FAILED;
    }

    if (strcmp(data_buf, str_ret_success) == 0)
    {
        free(data_buf);
        return 0;
    }

    memcpy(str_ret_failed, data_buf, trans_len);
    free(data_buf);
    return CTRL_ERR_OPERATE_FAILED;

}


/**
 * close a connection to LFTP ctrl server. make user can create a new connection.
 * @return 0 if success, otherwise errno.
 */
int ctrl_conn_close()
{
    static char exit_cmd[] = "exit\n";
    send(g_ctrl_conn_fd, exit_cmd, sizeof(exit_cmd), 0);
    int xno = 0;
    if (close(g_ctrl_conn_fd))
    {
        xno = errno;
    }
    g_ctrl_conn_fd = -1;
    return xno;
}

