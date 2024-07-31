# Non-blocking and blocking IO

## Socket programming

### Why we need to bind a socket to a `port`?

When the kernal recived the TCP packet, it needs to know which application should receive the packet. The `port` is used to identify the application.

### Why we need to bind a socket to an `address`?

In a machine, there can be multiple network interfaces. The `address` is used to identify the network interface.

Only if the `address` be bound to the socket, the socket can receive the packet from the kernal.

### How to check the socket is listening?

```bash
# Use netstat to check the status of the port 8080
netstat -an | grep 8080
```

### How `accept` function works?

When the TCP is connecting, the server side kernel will manage 2 queues:
- TCP half-open queue
    * The queue is not finished the 3-way handshake.
    * The server will be the `SYN-RECV` state.
- TCP full-open queue
    * The queue is finished the 3-way handshake.
    * The server will be the `ESTABLISHED` state.

When the TCP full queue is not empty, the server will use the `accept` function to get the connected socket from the queue and accept the connection.

> [!IMPORTANT]
> The listening socket is not the connected socket. The listening socket is used to listen to the port, and the connected socket is used to communicate with the client.

## How Blocking IO works?

When the server is listening to a port, we can use the `accept` function to accept the connection. The `accept` function will block the server until a client connects to the server.

During the blocking time, the server can't do anything else.

![Blocking IO](images/blocking_io.png)

