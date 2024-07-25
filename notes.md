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
int getaddrinfo(const char *node,
    const char *service,
    const struct addrinfo *hints,
    struct addrinfo **res);

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

## Connection

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
