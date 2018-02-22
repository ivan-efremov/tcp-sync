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

void TcpClient::setSocketOptions()
{
    int johnNagle = 1;
    int cork = 0;
    int maxBufferSize = 262144;
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
}

void TcpClient::connect()
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket < 0) {
        throw SystemException("Can't creates an endpoint for communication");
    }

    struct hostent *server = gethostbyname(m_host.c_str());
    if(server == NULL) {
        throw SystemException("No such host");
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
        throw SystemException("Can't connecting to");
    }
}

void TcpClient::send(const std::string& a_text)
{
    const char* buf = a_text.data();
    ssize_t     len = a_text.length();
    ssize_t     n   = 0L;
    do {
        n = ::send(m_socket, (void*)(buf + n), (size_t)len, MSG_NOSIGNAL);
        if(n >= 0L) {
            len -= n;
        } else {
            throw SystemException("Can't send data to server");
        }
    } while(len > 0L);
}

std::string TcpClient::recv()
{
    std::string text(4096, '\0');
    ssize_t n = ::recv(m_socket, (void*)text.data(), text.size(), 0);
    if(n >= 0L) {
        text.resize(n);
    } else {
        throw SystemException("Can't receive a data from server");
    }
    return text;
}

void TcpClient::close()
{
    if(m_socket != -1) {
        shutdown(m_socket, SHUT_RDWR);
        ::close(m_socket);
    }
}
