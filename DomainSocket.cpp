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

#include "DomainSocket.hh"

DomainSocket::DomainSocket(const std::string &path, DomainSocket::TYPE type) : _run(false), _type(type)
{
    // Prepare the socket structure
    _socket.sun_family = AF_UNIX;
    ::strncpy(_socket.sun_path, path.c_str(), path.length());

    // Get new socket fd
    if ((_fd = ::socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error(strerror(errno));

    // Server Socket
    if (_type == DomainSocket::SERVER)
    {
        // Remove old socket if exist
        ::unlink(path.c_str());

        // Connect the socket and start listening
        if (::bind(_fd, (struct sockaddr*)&_socket, path.length() + sizeof(_socket.sun_family)) == -1)
            throw std::runtime_error(strerror(errno));
        else if (::listen(_fd, 5) == -1)
            throw std::runtime_error(strerror(errno));
        else
            _run = true;
    }
    else if (_type == DomainSocket::CLIENT) // Client Socket
    {
        // Connect to the remote socket (a SERVER type socket)
        if (::connect(_fd, (struct sockaddr*)&_socket, path.length() + sizeof(_socket.sun_family)) == -1)
            throw std::runtime_error(strerror(errno));
        else
            _run =  true;
    }
    else
        throw std::runtime_error("Wrong constructor for a Server_Client socket.");
}

DomainSocket::DomainSocket(int fd, const sockaddr_un *socket) : _run(true), _type(DomainSocket::SERVER_CLIENT), _fd(fd)
{
    // Copy the content of the socket structure
    ::memcpy(&_socket, socket, strlen(socket->sun_path) + sizeof(socket->sun_family));
}

DomainSocket::~DomainSocket(void)
{
    if (_run)
        ::close(_fd);
}

DomainSocket *DomainSocket::acceptClient(void)
{
    int                 remoteFd;
    struct sockaddr_un  remoteSocket;
    socklen_t           size = sizeof(remoteSocket);

    // Only SERVER type socket can accept clients
    if (_type != DomainSocket::SERVER)
        throw std::runtime_error("This is not a server socket");

    // Accept the new client
    if ((remoteFd = ::accept(_fd, (struct sockaddr*)&remoteSocket, &size)) == -1)
        throw std::runtime_error(strerror(errno));

    // Create a SERVER_CLIENT socket to handle the client
    return new DomainSocket(remoteFd, &remoteSocket);
}


void DomainSocket::sendMsg(const std::string &msg)
{
    // SERVER type socket send msg to clients via SERVER_CLIENT socket
    if (_type == DomainSocket::SERVER)
        throw std::runtime_error("This is a server socket");

    // Send the msg
    if (send(_fd, msg.c_str(), msg.size(), 0) == -1)
        throw std::runtime_error(strerror(errno));
}

const std::string DomainSocket::recvMsg(void)
{
    char        buf[BUF_SIZE] = {0};
    std::string ret;
    int         len;

    // SERVER type socket receive msg to clients via SERVER_CLIENT socket
    if (_type == DomainSocket::SERVER)
        throw std::runtime_error("This is a server socket");

    // Receive the msg
    if ((len = recv(_fd, buf, BUF_SIZE, 0)) == -1)
        throw std::runtime_error(strerror(errno));
    else if (len == 0)                              // Client disconnected
        throw DomainSocket::Disconnected();

    ret.assign(buf, len);
    return ret;
}

void DomainSocket::closeSocket(void)
{
    if (_run)
        close(_fd);
}


/* GETTERS / SETTERS */
bool DomainSocket::isRunning(void) const { return _run; }

int DomainSocket::fd(void) const { return _fd; }

const sockaddr_un *DomainSocket::socket(void) const { return &_socket; }
