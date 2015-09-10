# DomainSocket
Server - Client communication using unix domain socket

Functionnality
--------------
DomainSocket class provide an abstraction of Unix Domain socket.
You can create server socket and client socket to communicate.
There is a Daemon class and a Sender class (Server and Client) for an example of usage.

Example
-------
You can compile the example (c++11 required for the for loop) and test by yourself.
The server provided is just a basic echo server.

```
make
./server my_socket &
./client my_socket "This is a test message"
This is a test message : Sended to server !
Server response : This is a test message
```

