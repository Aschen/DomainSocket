/*
 This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

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
