#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "ctrl.h"
#include "repl_loop.h"

int main(int argc, char** argv)
{

    if (argc == 1)
    {
        main_loop(10001);
        return 0;
    }


    return 0;
}
