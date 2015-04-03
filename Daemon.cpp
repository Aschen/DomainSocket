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

#include "Daemon.hh"

Daemon::Daemon(const std::string &path)
    : _local(path, DomainSocket::SERVER), _run(false)
{    
}

Daemon::~Daemon(void)
{
    for (DomainSocket::iterator it = _clients.begin; it < _clients.end(); ++it)
    {
        delete *it;
    }
}

void Daemon::start(void)
{
    _run = true;
    while (_run)
    {
        handleSockets();
    }
    std::cout << "Server shutdown" << std::endl;
}

void Daemon::handleSockets(void)
{
    fd_set              readfds;
    fd_set              writefds;
    int                 fd_max;
    struct timeval      tv;

    fd_max = initSelect(&tv, &readfds, &writefds);
    if (::select(fd_max, &readfds, NULL, NULL, &tv) == -1)
    {
        throw std::runtime_error(std::string("Select Error : ") + ::strerror(errno));
    }
    else
    {
        // If something to read on stdin
        if (FD_ISSET(0, &readfds))
            eventTerminal();
        // If new client connect
        if (FD_ISSET(_local.fd(), &readfds))
            eventServer();
        // Check clients's socket
        eventClients(&readfds, &writefds);
    }

}

int Daemon::initSelect(struct timeval *tv, fd_set *readfds, fd_set *writefds)
{
    int     fd_max = _local.fd();

    // Timeout 100 ms
    tv->tv_sec = 0;
    tv->tv_usec = 100;

    // Initialize bits field for select
    FD_ZERO(readfds);
    FD_SET(_local.fd(), readfds);
    FD_SET(0, readfds);
    if (writefds != NULL)
    {
        FD_ZERO(writefds);
        FD_SET(_local.fd(), writefds);
    }
    for (DomainSocket *client : _clients)
    {
        FD_SET(client->fd(), readfds);
        if (writefds != NULL)
            FD_SET(client->fd(), writefds);
        // Check if client's fd is greater than actual fd_max
        fd_max = (fd_max < client->fd()) ? client->fd() : fd_max;
    }
    return fd_max + 1;
}

void Daemon::eventTerminal(void)
{
    std::string     msg;

    std::cin >> msg;
    if (msg == "exit")
    {
        _run = false;
    }
}

void Daemon::eventServer(void)
{
    DomainSocket    *client;

    try
    {
        client = _local.acceptClient();
        _clients.push_back(client);
    }
    catch (std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }
}

void Daemon::eventClients(fd_set *readfds, fd_set *writefds)
{
    std::string         msg;

    for (std::vector<DomainSocket*>::iterator it = _clients.begin(); it < _clients.end(); ++it)
    {
        // Something to write on client socket
        if (FD_ISSET((*it)->fd(), writefds))
        {
            if (_msgs.size())
            {
                try
                {
                    (*it)->sendMsg(_msgs.back());
                    _msgs.pop_back();
                }
                catch (std::runtime_error &e)
                {
                    std::cout << e.what() << std::endl;
                }
            }
        }
        // Something to read on client socket
        if (FD_ISSET((*it)->fd(), readfds))
        {
            try
            {
                _msgs.push_back((*it)->recvMsg());
            }
            catch (DomainSocket::Disconnected &e)
            {
                delete (*it);
                _clients.erase(it);
                it = _clients.begin();
            }
            catch (std::runtime_error &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }
}

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "Usage : " << av[0] << " <socket_path>" << std::endl;
        return 1;
    }

    try
    {
        Daemon      d(av[1]);

        d.start();
    }
    catch (std::runtime_error &e)
    {

    }

    return 0;
}
