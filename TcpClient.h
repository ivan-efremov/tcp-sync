/**
 * @file TcpClient.h
 */
#pragma once

#include <string>
#include <memory>
#include <netinet/in.h>

/**
 * class TcpClient.
 * @short Синхронный TCP клиент.
 */
class TcpClient
{
public:
                       TcpClient(const std::string& a_host, int a_port);
                      ~TcpClient();
    void               send(const std::string& a_text);
    std::string        recv();
    void               connect();
    void               close();
private:
    std::string        m_host;
    int                m_port;
    int                m_socket;
    struct sockaddr_in m_sockaddr;
};

typedef std::shared_ptr<TcpClient> PTcpClient;

