#pragma once

#include "../../posix/socket.h"
#include "../../posix/streambuf.h"

class MiniHttpServer
{
    embr::posix::Socket ss;

public:
    int start(int port)
    {
        struct sockaddr_in server_addr;
        int server_fd;

        if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            // fails on windows, presumably a permission thing... except we aren't even listening yet
            perror("socket failed");
            return EXIT_FAILURE;
        }

        // Guidance from:
        // https://stackoverflow.com/questions/10619952/how-to-completely-destroy-a-socket-connection-in-c
        // We're trying to flush accepted socket contents.  This doesn't seem to help, though
        int v = 1;
        setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&v,sizeof(v));

        server_addr.sin_family = AF_INET;
    #if __WIN64__
        server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    #else
        server_addr.sin_addr.s_addr = INADDR_ANY;
    #endif
        server_addr.sin_port = htons(port);

        ss = server_fd;

        if(ss.bind(&server_addr) < 0)
        {
            perror("bind failed");
            return EXIT_FAILURE;
        }

        if(listen(server_fd, 3) < 0)
        {
            perror("listen failed");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    embr::posix::Socket accept()
    {
        struct sockaddr_in client_addr;

        socklen_t client_addr_size = sizeof(client_addr);

        return ss.accept(&client_addr, &client_addr_size);
    }

    void test1()
    {
        ss.close();
    }
};


// As per [1]
template <class Streambuf, class Base>
void http_respond_ok(estd::detail::basic_ostream<Streambuf, Base>& out)
{
    out << "HTTP/1.1 200 OK" << HTTP_ENDL;
    out << "Content-Type: text/plain; version=0.0.4" << HTTP_ENDL;
    out << "Connection: close" << HTTP_ENDL;
    out << HTTP_ENDL;
}


