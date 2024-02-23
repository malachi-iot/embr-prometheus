#include <iostream>

#if __WIN64__
#include <winsock2.h>
#else
#include <inaddr.h>
#include <sys/unistd.h>
#endif

//#include <embr/prometheus.h>
#include <embr/internal/histogram.h>

using namespace std;

// Guidance from https://dev.to/jeffreythecoder/how-i-built-a-simple-http-server-from-scratch-using-c-739

int main()
{
    int server_fd;
    struct sockaddr_in server_addr;

    cout << "Hello World!" << endl;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // fails on windows, presumably a permission thing... except we aren't even listening yet
        perror("socket failed");
        return EXIT_FAILURE;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    server_addr.sin_port = 80;

    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        return EXIT_FAILURE;
    }

    if(listen(server_fd, 10) < 0)
    {
        perror("listen failed");
        return EXIT_FAILURE;
    }

    return 0;
}
