# Builtin Functions Description

## Network Protocol

### addrinfo

```c++
struct addrinfo
{
    int ai_flags;               // control options
    int ai_family;              // address family
    int ai_socktype;            // socket type
    int ai_protocol;            // protocol (e.g. TCP, UDP)
    size_t ai_addrlen;          // length of ai_addr
    struct sockaddr *ai_addr;   // address of the socket
    char *ai_canonname;         // canonical name of the host
    struct addrinfo *ai_next;   // next addrinfo struct
};
```

### getaddrinfo

Use to analyze the host and service arguments and to fill in the addrinfo structure.

Use `getaddrinfo` to get the address information of a host. The function returns a linked list of `addrinfo` structures. The function takes the following arguments:

```c++
int getaddrinfo(const char *node,   // host or IP address
    const char *service,            // port or service name
    const struct addrinfo *hints,
    struct addrinfo **res);         // result

// Example
struct addrinfo hints;
struct addrinfo *res;
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;
getaddrinfo(NULL, "8080", &hints, &res);
```

### freeaddrinfo

Use to free the memory allocated by `getaddrinfo`.

```c++
void freeaddrinfo(struct addrinfo *res);
```


### gai_strerror

Similarly, the function `perror` can be used to get the error message of the system error code.

If the function fails, it returns a string describing the error.
Something like "EAI_AGAIN", "EAI_BADFLAGS", "EAI_FAIL", "EAI_FAMILY", "EAI_MEMORY", "EAI_NONAME", "EAI_SERVICE", "EAI_SOCKTYPE".

```c++
const char *gai_strerror(int errcode);
```

### getprotobyname

Use to get the protocol information by the protocol name.

```c++
struct protoent {
    char *p_name;     // protocol name
    char **p_aliases; // protocol aliases
    int p_proto;      // protocol number
};
```

```c++
struct protoent *getprotobyname(const char *name);

// Example
struct protoent *proto = getprotobyname("tcp");
int protocol = proto->p_proto;
```

## Connection

### accept

### listen

### send

### recv

### bind

### connect


## Socket

### socket

Use to create a socket for network communication.

```c++
int socket(int domain, int type, int protocol);
```

* `domain`: The address family of the socket. It can be `AF_INET` for IPv4, `AF_INET6` for IPv6, `AF_UNIX` for Unix domain sockets, `AF_UNSPEC` for unspecified.
* `type`: The type of the socket. It can be `SOCK_STREAM` for TCP, `SOCK_DGRAM` for UDP, `SOCK_RAW` for raw socket.
* `protocol`: The protocol of the socket. It can be `0` for unspecified and it will auto detect, `IPPROTO_TCP` for TCP, `IPPROTO_UDP` for UDP.
* Return: The file descriptor of the socket. If the function fails, it returns `-1` and sets `errno`.

### socketpair

Use to create a pair of connected sockets for inter-process communication (IPC).

```c++
int socketpair(int domain, int type, int protocol, int sv[2]);
```

* `domain`: The address family of the socket. Usually, it is `AF_UNIX` for Unix domain sockets.
* `type`: Same as the `socket` function.
* `protocol`: Same as the `socket` function.
* `sv`: The file descriptors of the two sockets. The first one is for reading and the second one is for writing.
* Return: If the function fails, it returns `-1` and sets `errno`.

### setsockopt

Use to control the behavior of the socket. Like setting the timeout, reusing the address, etc.

```c++
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

* `sockfd`: The file descriptor of the socket.
* `level`: The protocol level at which the option resides. It can be `SOL_SOCKET` for socket-level options, `IPPROTO_TCP` for TCP-level options, `IPPROTO_IP` for IP-level options.
* `optname`: The option to set. It can be `SO_REUSEADDR` to reuse the address, `SO_REUSEPORT` to reuse the port, `SO_RCVBUF` to set the receive buffer size, `SO_SNDBUF` to set the send buffer size, `SO_RCVTIMEO` to set the receive timeout, `SO_SNDTIMEO` to set the send timeout, `SO_KEEPALIVE` to keep the connection alive, `SO_LINGER` to set the linger time, `SO_BROADCAST` to allow broadcasting, `SO_OOBINLINE` to receive out-of-band data inline, `SO_NO_CHECK` to disable checksum, `SO_PRIORITY` to set the priority, `SO_RCVLOWAT` to set the receive low water mark, `SO_SNDLOWAT` to set the send low water mark.
* `optval`: The value of the option.
* `optlen`: The length of the option value.

```c++
// Example: Set the timeout of the socket
struct timeval timeout;
timeout.tv_sec = 5;
timeout.tv_usec = 0;
setsockopt(
    sockfd,         // The file descriptor of the socket
    SOL_SOCKET,     // The protocol level
    SO_RCVTIMEO,    // The option to set, receive timeout
    &timeout,       // The value of the option
    sizeof(timeout) // The length of the option value
);
```

### getsockname

Use to get the local address of the socket.

```c++
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

* `sockfd`: The file descriptor of the socket.
* `addr`: The address of the socket.
* `addrlen`: The length of the address.

### Other Macros

Using `SOMAXCONN` to get the maximum number of connections that can be queued by `listen`.

Using `SOL_SOCKET` to set socket-level options.

Using `SO_REUSEADDR` to reuse the address.

Using `SO_REUSEPORT` to reuse the port.

Using `SO_RCVBUF` to set the receive buffer size.

Using `SO_SNDBUF` to set the send buffer size.

Using `SO_RCVTIMEO` to set the receive timeout.

Using `SO_SNDTIMEO` to set the send timeout.

Using `SO_ERROR` to get the error status of the socket.

Using `SO_KEEPALIVE` to keep the connection alive.

Using `SO_LINGER` to set the linger time.

Using `SO_BROADCAST` to allow broadcasting.

Using `SO_OOBINLINE` to receive out-of-band data inline.

Using `SO_NO_CHECK` to disable checksum.

Using `SO_PRIORITY` to set the priority.

Using `SO_RCVLOWAT` to set the receive low water mark.

## I/O Multiplexing

### fcntl

For the webserver to be able to handle multiple connections, loggers and make sure the fd would not be inherited by the child process, we need to use `fcntl` to control the file descriptor.

#### 1. For setting the file descriptor to non-blocking mode.

Non-blocking means that the read and write operations on the file descriptor will return immediately if there is no data available.

```c++
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0); // get the current flags
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return -1;
    }

    flags |= O_NONBLOCK;                // set the non-blocking flag
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl(F_SETFL)");
        return -1;
    }
    return 0;
}
```

#### 2. Lock the file descriptor to avoid multiple processes writing to the same file.

```c++
int lock_file(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;        // write lock
    fl.l_whence = SEEK_SET;     // relative to beginning of file
    fl.l_start = 0;             // start from 0
    fl.l_len = 0;               // lock the whole file

    if (fcntl(fd, F_SETLKW, &fl) == -1)
    {
        perror("fcntl(F_SETLKW)");
        return -1;
    }
    return 0;
}

int unlock_file(int fd) {
    struct flock fl;
    fl.l_type = F_UNLCK;        // unlock
    fl.l_whence = SEEK_SET;     // relative to beginning of file
    fl.l_start = 0;             // start from 0
    fl.l_len = 0;               // unlock the whole file

    if (fcntl(fd, F_SETLK, &fl) == -1)
    {
        perror("fcntl(F_SETLK)");
        return -1;
    }
    return 0;
}
```

#### 3. Handle the inherited in the child process.

```c++
int close_on_exec(int fd)
{
    int flags = fcntl(fd, F_GETFD, 0);      // get the current flags
    if (flags == -1) {
        perror("fcntl(F_GETFD)");
        return -1;
    }

    // set the close-on-exec flag, the file descriptor will be closed when the process is replaced by another process
    flags |= FD_CLOEXEC;
    if (fcntl(fd, F_SETFD, flags) == -1)
    {
        perror("fcntl(F_SETFD)");
        return -1;
    }
    return 0;
}
```

## Async

### select

### poll

Use to monitors an array of file descriptors.

Enabling a single-threaded program to manage multiple I/O streams concurrently without resorting to blocking operations or spinning in a busy-wait loop.

```c++
int poll(struct pollfd fds[], nfds_t nfds, int timeout);
```
* `fds`: Array of pollfd structures.
* `nfds`: Number of file descriptors.
* `timeout`: Maximum time to wait (in milliseconds). A value of -1 means an infinite timeout, 0 means non-blocking.
* Return: returns the number of file descriptors that have events or -1 on error. The revents field of the pollfd structures is updated to reflect the events that occurred.

## epoll

### epoll_create

### epoll_ctl

### epoll_wait

## kqueue

### kqueue

### kevent


## Network Byte Order and Host Byte Order Conversion

* Host byte order: The byte order used by the host. It can be big-endian or little-endian.
* Network byte order: The byte order used by the network. It is big-endian.

When sending data over the network, it is important to convert the data to network byte order. The network byte order is big-endian, while the host byte order can be big-endian or little-endian. The following functions can be used to convert between network byte order and host byte order.

### htons

Host TO Network Short, convert a short integer from host byte order to network byte order.

```c++
uint16_t htons(uint16_t hostshort);

// Example
uint16_t port = 8080;
uint16_t network_port = htons(port);
```

### htonl

Host TO Network Long, convert a long integer from host byte order to network byte order.

```c++
uint32_t htonl(uint32_t hostlong);

// Example
uint32_t ip = 0x7f000001;
uint32_t network_ip = htonl(ip);
```

### ntohs

Network TO Host Short, convert a short integer from network byte order to host byte order.

```c++
uint16_t ntohs(uint16_t netshort);

// Example
uint16_t network_port = 8080;
uint16_t port = ntohs(network_port);
```

### ntohl

Network TO Host Long, convert a long integer from network byte order to host byte order.

```c++
uint32_t ntohl(uint32_t netlong);

// Example
uint32_t network_ip = 0x7f000001;
uint32_t ip = ntohl(network_ip);
```

## Macros

### FD_CLR

Using `FD_CLR` to clear a file descriptor from a set.

```c++
void FD_CLR(int fd, fd_set *set);
```

### FD_ISSET

Using `FD_ISSET` to check if a file descriptor is in a set.

```c++
int FD_ISSET(int fd, fd_set *set);
```

### FD_SET

Using `FD_SET` to add a file descriptor to a set.

```c++
void FD_SET(int fd, fd_set *set);
```

### FD_ZERO

Using `FD_ZERO` to clear all file descriptors from a set.

```c++
void FD_ZERO(fd_set *set);
```

### INADDR_ANY

Using `INADDR_ANY` to bind to all interfaces.

```c++
#define INADDR_ANY ((in_addr_t) 0x00000000)
```

### INADDR_LOOPBACK

Using `INADDR_LOOPBACK` to bind to the loopback interface.

```c++
#define INADDR_LOOPBACK ((in_addr_t) 0x7f000001)
```

### INADDR_NONE

Using `INADDR_NONE` to indicate an invalid address.

```c++
#define INADDR_NONE ((in_addr_t) 0xffffffff)
```

### INET_ADDRSTRLEN

Using `INET_ADDRSTRLEN` to get the maximum length of an IPv4 address string.

### INET6_ADDRSTRLEN

Using `INET6_ADDRSTRLEN` to get the maximum length of an IPv6 address string.

### select

### poll

## epoll

### epoll_create

### epoll_ctl

### epoll_wait

## kqueue

### kqueue

### kevent

### SO_SNDLOWAT

