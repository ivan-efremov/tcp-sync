/**
 * @file TcpClient.h
 */
#pragma once

#include <string>

/**
 * class TcpClient.
 * @short Синхронный TCP клиент.
 */
class TcpClient
{
public:
                        TcpClient(const std::string& a_host, int a_port);
    virtual void        send(const std::string& a_text);
    virtual std::string recv();
    virtual void        connect();
    virtual void        close();
protected:
    virtual void        hostResolve(const std::string& a_host);
private:
    std::string         m_host;
    int                 m_port;
};
