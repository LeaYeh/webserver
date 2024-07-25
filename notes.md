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
