#include "TcpClient.h"
#include "Exception.h"
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>



TcpClient::TcpClient(const std::string& a_host, int a_port)
    : m_host(a_host),
      m_port(a_port),
      m_socket(-1)
{
}

TcpClient::~TcpClient()
{
    close();
}

std::string TcpClient::getHost() const
{
    return m_host;
}

int TcpClient::getPort() const
{
    return m_port;
}

int TcpClient::getSocket() const
{
    return m_socket;
}

bool TcpClient::isOpen() const
{
    int error = 0;
    socklen_t errorLen = sizeof(error);
    return m_socket > 0 &&
           getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &error, &errorLen) != -1 &&
           error == 0;
}

void TcpClient::setSocketOptions()
{
    int johnNagle = 1;
    int cork = 0;
    int maxBufferSize = BUFFER_SIZE;
    int keepALive = 1;
    struct timeval timeout = { 60, 0 };
    if(setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
        throw SystemException("Can't setsockopt SO_RCVTIMEO");
    }
    if(setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
        throw SystemException("Can't setsockopt SO_SNDTIMEO");
    }
    if(setsockopt(m_socket, SOL_TCP/*IPPROTO_TCP*/, TCP_NODELAY, (char*)&johnNagle, sizeof(johnNagle)) == -1) {
        throw SystemException("Can't setsockopt TCP_NODELAY");
    }
    if(setsockopt(m_socket, SOL_TCP, TCP_CORK, (char*)&cork, sizeof(cork)) == -1) {
        throw SystemException("Can't setsockopt TCP_CORK");
    }
    if(setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&maxBufferSize, sizeof(maxBufferSize)) == -1) {
        throw SystemException("Can't setsockopt SO_RCVBUF");
    }
    if(setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char*)&maxBufferSize, sizeof(maxBufferSize)) == -1) {
        throw SystemException("Can't setsockopt SO_SNDBUF");
    }
    if(setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepALive, sizeof(keepALive)) == -1) {
        throw SystemException("Can't setsockopt SO_KEEPALIVE");
    }
}

void TcpClient::connect()
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket < 0) {
        throw SystemException("Can't creates an endpoint for communication");
    }

    struct hostent *server = gethostbyname(m_host.c_str());
    if(server == NULL) {
        throw SystemException(std::string("No such host: ") + m_host);
    }

    bzero((char*)&m_sockaddr, sizeof(m_sockaddr));
    m_sockaddr.sin_family = AF_INET;
    bcopy(
        (char*)server->h_addr,
        (char*)&m_sockaddr.sin_addr.s_addr,
        server->h_length
    );
    m_sockaddr.sin_port = htons(m_port);

    setSocketOptions();

    if(::connect(m_socket, (struct sockaddr*)&m_sockaddr, sizeof(m_sockaddr)) < 0) {
        throw SystemException(
                std::string("Can't connecting to server: ") + m_host + ":" + std::to_string(m_port)
            );
    }
}

void TcpClient::send(const std::string& a_text)
{
    const char* buf = a_text.data();
    ssize_t     len = a_text.length();
    ssize_t     n   = 0L;
    do {
        ssize_t res = ::send(m_socket, (void*)(buf + n), (size_t)len, MSG_NOSIGNAL);
        if(res > 0L) {
            n += res;
            len -= res;
        } else {
            throw SystemException(
                    std::string("Can't send data to server: " + m_host + ":" + std::to_string(m_port))
                );
        }
    } while(len > 0L);
}

std::string TcpClient::recv()
{
    char buf[BUFFER_SIZE];
    ssize_t n = ::recv(m_socket, (void*)buf, BUFFER_SIZE, 0);
    if(n <= 0L) {
        throw SystemException(
                std::string("Can't receive a data from server: " + m_host + ":" + std::to_string(m_port))
            );
    }
    return {buf, (size_t)n};
}

void TcpClient::close()
{
    if(m_socket != -1) {
        ::shutdown(m_socket, SHUT_RDWR);
        ::close(m_socket);
        m_socket = -1;
    }
}


/**
 * struct TcpClient::GetLine.
 */
std::string TcpClient::GetLine::operator()(PTcpClient a_tcpClient)
{
    std::string s;
    size_t n;
    while((n = m_line.find("\r\n")) == std::string::npos) {
        m_line += a_tcpClient->recv();
    }
    s = m_line.substr(0, n);
    m_line.erase(0, n + 2);
    return s;
}
