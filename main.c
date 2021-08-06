#include <stdio.h>
#include <errno.h>
#include "ctrl.h"

int main(int argc, char** argv)
{

    int ret = ctrl_conn_create(10001);

    if (ret)
    {
        printf("ERRNO: %d", ret);
        return ret;
    }

//    ctrl_send_file("/home/alvis/workspace/LAN-File-Transfer-Java/README.md", "127.0.0.1", "10000");
    ctrl_set_conf("save-path", "/home/alvis/download");


    ctrl_conn_close();

    return 0;
}
