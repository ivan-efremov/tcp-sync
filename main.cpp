/**
 * @file main.cpp
 */

#include "Exception.h"
#include "TcpClient.h"
#include <iostream>



int main(int argc, const char *argv[])
{
    try {
        PTcpClient tcpClient(std::make_shared<TcpClient>("ya.ru", 80));
        tcpClient->connect();
        tcpClient->send(
            "GET / HTTP/1.1\r\n"
            "User-Agent: MyBrowser/0.1\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
            "Accept-Language: en-US,en;q=0.5\r\n"
            "Accept-Encoding: gzip, deflate, br\r\n"
            "Connection: close\r\n"
            "\r\n"
        );
        TcpClient::GetLine getLine;
        std::string line;
        do {
            line = getLine(tcpClient);
            std::cout << line.length() << ": " << line << std::endl;
        } while(!line.empty());
        tcpClient->close();
    } catch(const std::exception& err) {
        std::cerr << "ERROR: " << err.what() << std::endl;
    }
    return 0;
}
