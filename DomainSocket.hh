#ifndef DOMAINSOCKET_HH
#define DOMAINSOCKET_HH

#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <iostream>
#include <stdexcept>

# define BUF_SIZE 512

class DomainSocket
{
public:
    // Handle client disconnection
    class Disconnected : public std::exception
    {
    public:
        Disconnected(void) { }
        ~Disconnected(void) throw() { }
    };

    // 3 types of DomainSocket
public:
    typedef enum    e_type
    {
        SERVER          = 0, // The server part
        SERVER_CLIENT,       // Used by the server to handle new clients
        CLIENT               // Client that actually connect to the server
    }               TYPE;

private:
    bool                        _run;
    DomainSocket::TYPE          _type;
    int                         _fd;
    struct sockaddr_un          _socket;

public:
    // Server or Client socket
    DomainSocket(const std::string &path, TYPE type);
    // Server_client socket
    DomainSocket(int fd, const struct sockaddr_un *socket);

    ~DomainSocket(void);

    // Accept a new client and return a DomainSocket of type SERVER_CLIENT
    DomainSocket                *acceptClient(void);
    // Send a msg throught the socket (Only SERVER_CLIENT or CLIENT socket)
    void                        sendMsg(const std::string &msg);
    // Receive a msg throught the socket (Only SERVER_CLIENT or CLIENT socket)
    const std::string           recvMsg(void);
    // Close the socket
    void                        closeSocket(void);

public:
    bool                        isRunning(void) const;
    int                         fd(void) const;
    const struct sockaddr_un    *socket(void) const;
};

#endif // DOMAINSOCKET_HH
