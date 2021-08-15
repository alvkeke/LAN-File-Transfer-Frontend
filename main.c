#include <stdio.h>
#include <stdlib.h>

#include <malloc.h>
#include <string.h>
#include "ctrl.h"
#include "repl_loop.h"


enum CmdState{
    NONE,
    IPADDRESS,
    PORT,
};

int parsePort(char *str)
{
    int rst = 0;
    for (int i = 0; i < strlen(str); ++i)
    {
        if (str[i] < '0' || str[i] >'9') return -1;
        rst*=10;
        rst+= str[i]-'0';
    }

    return rst;
}

int main(int argc, char** argv)
{

    int cmd_state = NONE;
    int port = -1;
    int i_sip = -1;
    int i_cmd = -1;

    for (int i = 1; i < argc; ++i)
    {
        if (cmd_state == IPADDRESS)
        {
            if (i_sip >= 0) continue;
            i_sip = i;
        }
        else if (PORT == cmd_state)
        {
            if (port > 0) continue;
            port = parsePort(argv[i]);
        }

        if (cmd_state != NONE)
        {
            cmd_state = NONE;
            continue;
        }

        if (strcmp("-d", argv[i]) == 0)
        {
            cmd_state = IPADDRESS;
        }
        else if (strcmp("-p", argv[i]) == 0)
        {
            cmd_state = PORT;
        }
        else
        {
            if (i_cmd >= 0) continue;
            i_cmd = i;
        }
    }

    if (port <0) port = 10001;
    char *sip;
    if (i_sip<0)
        sip = "127.0.0.1";
    else
        sip = argv[i_sip];


    if (i_cmd<0)
        main_loop_raw(sip, port);
    else
    {

        if (strcmp(argv[i_cmd], "exit") == 0) return 0;
        int ret = ctrl_conn_create(sip, port);
        if (ret) {
            printf("ERROR: cannot create connection: %d\n", ret);
            return ret;
        }

        ctrl_exec_raw(argv[i_cmd], 1);

        ctrl_conn_close();
    }

    return 0;


}
