//
// Created by alvis on 2021/8/7.
//

#include "repl_loop.h"
#include "ctrl.h"

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void clear_cli_line()
{
    printf("\x1b[2K");
    printf("\x1b[1G");
}

void print_cli_format()
{
    printf("%s%s",CLI_NAME ,CLI_BREAKER);
}


void echo_disable()
{
    struct termios attr;
    tcgetattr(fileno(stdin), &attr);

    attr.c_lflag &= ~ECHO;
    tcsetattr(fileno(stdin), 0, &attr);

}

void echo_enable()
{
    struct termios attr;
    tcgetattr(fileno(stdin), &attr);

    attr.c_lflag = ECHO;
    tcsetattr(fileno(stdin), 0, &attr);
}


size_t cmd_param_len(char *cmd)
{
    int i;
    for (i=0; i< strlen(cmd); i++)
    {
        if (cmd[i] == ' ') break;
    }
    return i;
}

typedef struct cmd_array {
    char *data;
    struct cmd_array *next;
}* cmd_list_t;

cmd_list_t cmd_list_gen(char *cmd_line)
{
    struct cmd_array *param_list = NULL;
    char *begin = cmd_line;

    while (*begin == ' ') begin++;

    while(*begin != 0x00)
    {
        struct cmd_array* ptr = (struct cmd_array*) malloc(sizeof(struct cmd_array));
        if (!ptr)
        {
            printf("[W] : no memory.\n");
            break;
        }

        size_t len = cmd_param_len(begin);

        ptr->data = (char*) malloc(len+1);
        if (!ptr->data)
        {
            printf("[W] : no memory.\n");
            break;
        }

        memcpy(ptr->data, begin, len);
        ptr->data[len] = 0x00;
        ptr->next = NULL;

        if (param_list)
        {
            struct cmd_array* itr = param_list;
            while(itr->next != NULL) itr = itr->next;
            itr->next = ptr;
        }
        else
        {
            param_list = ptr;
        }

        begin += len;

        while (*begin == ' ') begin++;
    }

    return param_list;
}

void cmd_list_release(cmd_list_t array)
{
    while (array != NULL)
    {
        struct cmd_array *itr = array;
        array = array->next;
        free(itr->data);
        free(itr);
    }
}

void cmd_list_print(cmd_list_t array)
{
    for (cmd_list_t itr = array; itr!=NULL; itr=itr->next)
    {
        puts(itr->data);
    }
}

int cmd_list_count(cmd_list_t array)
{
    int count = 0;
    for (cmd_list_t itr = array; itr!=NULL; itr=itr->next)
    {
        count++;
    }

    return count;
}

int cmd_list_exec(cmd_list_t array)
{
    int count = cmd_list_count(array);

    if (strcmp(array->data, "send") == 0)
    {
        printf("try to send file:");
        if (count < 4)
        {
            printf("failed: lack of parameters\n");
            return 0;
        }
        int ret = ctrl_send_file(array->next->data, array->next->next->data, array->next->next->next->data);
        if (ret)
            printf("failed: %d\n", ret);
        else
            printf("success.\n");

    }
    else if (strcmp(array->data, "set") == 0)
    {
        printf("try to set configuration:");
        if (count < 3)
        {
            printf("failed: lack of parameters\n");
            return 0;
        }

        int ret = ctrl_set_conf(array->next->data, array->next->next->data);
        if (ret)
            printf("failed: %d\n", ret);
        else
            printf("success.\n");
    }
    else if (strcmp(array->data, "scan") == 0)
    {
        printf("Sorry, Scan was not implemented, run as a raw command.\n");
        return 1;
    }
    else
    {
        printf("unknown command, raw cmd");
        return 1;
    }
    return 0;
}

void main_loop(const char *ip, int port)
{
    static char cmd_line[CMD_LINE_MAX];

    if (ctrl_conn_create(ip, port)) return;

    while(1)
    {
        print_cli_format();

        fgets(cmd_line, CMD_LINE_MAX, stdin);
        size_t read_len = strlen(cmd_line);
        cmd_line[read_len-1] = 0;

        if (strcmp(cmd_line, "exit") == 0) break;

        cmd_list_t array = cmd_list_gen(cmd_line);
        if (cmd_list_exec(array))
            ctrl_exec_raw(cmd_line, 0);

        cmd_list_release(array);
    }

    ctrl_conn_close();

}

void main_loop_raw(const char *ip, int port)
{
    static char cmd_line[CMD_LINE_MAX];

    if (ctrl_conn_create(ip, port)) return;

    while(1)
    {
        print_cli_format();

        fgets(cmd_line, CMD_LINE_MAX, stdin);
        size_t read_len = strlen(cmd_line);
        cmd_line[read_len-1] = 0;

        if (strcmp(cmd_line, "exit") == 0) break;

        ctrl_exec_raw(cmd_line, 1);

    }

    ctrl_conn_close();

}
