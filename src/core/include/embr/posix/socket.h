#pragma once

#if __WIN64__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/ip.h>
#endif

#include <sys/unistd.h>

namespace embr { inline namespace posix {

// this is the common read/write stuff, useful for pipes, files, etc.
class SocketBase
{
protected:
    int fd_;

    constexpr SocketBase(int fd) : fd_{fd}  {}

public:
    constexpr bool valid() const { return fd_ >= 0; }

    int close() const { return ::close(fd_); }

    ssize_t write(const void* buf, size_t n) const
    {
        return ::write(fd_, buf, n);
    }

    operator int() const { return fd_; }
};

// A lot of existing wrappers out there, but they are too fancy.  Don't want pthreads or c++17
// dependency (although the latter is not bad)
// NOTE: I KNOW I've made a socket wrapper before... just... where is it now?
class Socket : public SocketBase
{
    using base_type = SocketBase;

public:
    constexpr explicit Socket(int fd) : base_type(fd) {}
    Socket(int domain, int type, int protocol = 0) :
        base_type{socket(domain, type, protocol)}
    {

    }

    template <class Addr>
    int bind(const Addr* addr) const
    {
        return ::bind(fd_, (struct sockaddr *)addr, sizeof(Addr));
    }

    int listen(int count) const
    {
        return ::listen(fd_, count);
    }

    template <class Addr>
    Socket accept(Addr* addr, socklen_t* len) const
    {
        return Socket(::accept(fd_, (struct sockaddr *) addr, len));
    }
};

}}
