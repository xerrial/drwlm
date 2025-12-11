//
// DRWLM Daemon
//
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

const char *usage = "Usage: drwlm [-f] [-h]\n"
                    "  -f    run in foreground\n"
                    "  -h    show this help\n";

int become_daemon()
{
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    int  opt       = -1;
    bool daemonize = true;

    while ((opt = getopt(argc, argv, "fh")) != -1)
    {
        switch (opt)
        {
        case 'f':
            daemonize = false;
            break;
        case 'h':
            // print usage
            return EXIT_SUCCESS;
        default:
            // print usage
            return EXIT_FAILURE;
        }
    }

    if (daemonize && become_daemon() != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
