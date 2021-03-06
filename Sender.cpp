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

#include "Sender.hh"

Sender::Sender(const std::string &path, const std::string &msg)
    : _local(path, DomainSocket::CLIENT)
{
    if (_local.isRunning())
    {
        try
        {
            _local.sendMsg(msg);
            std::cout << msg << " : Sended to server !" << std::endl;
            std::cout << "Server response : " << _local.recvMsg() << std::endl;
        }
        catch (std::runtime_error &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
}

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cout << "Usage : " << av[0] << " <socket_path> <msg>" << std::endl;
        return 1;
    }

    try
    {
        Sender      s(av[1], av[2]);
    }
    catch (std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
