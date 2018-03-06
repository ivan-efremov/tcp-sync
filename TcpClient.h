/**
 * @file TcpClient.h
 */
#pragma once

#include <string>
#include <memory>
#include <netinet/in.h>


class TcpClient;
typedef std::shared_ptr<TcpClient> PTcpClient;


/**
 * class TcpClient.
 * @short Синхронный TCP клиент.
 */
class TcpClient
{
    enum {
        BUFFER_SIZE = 524288
    };
public:
    struct GetLine {
        std::string m_line;
        std::string operator()(PTcpClient a_tcpClient);
    };
public:
                        TcpClient(const std::string& a_host, int a_port);
    virtual            ~TcpClient();
    virtual void        send(const std::string& a_text);
    virtual std::string recv();
    virtual void        connect();
    virtual void        close();
    std::string         getHost() const;
    int                 getPort() const;
    int                 getSocket() const;
    bool                isOpen() const;
protected:
    virtual void        setSocketOptions();
protected:
    const std::string   m_host;
    const int           m_port;
    int                 m_socket;
    struct sockaddr_in  m_sockaddr;
};
