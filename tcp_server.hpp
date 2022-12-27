//
//  tcp_server.hpp
//  web_server
//
//  Created by Сарапулов Михаил on 27.12.2022.
//

#ifndef tcp_server_hpp
#define tcp_server_hpp

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>

namespace http
{
class TcpServer
{
public:
    TcpServer(std::string ipAddress, int port);
    ~TcpServer();
    void startListening();
    
private:
    // propeties
    std::string lIpAddress;
    int lSocket;
    int incomingSocket;
    int lPort;
    unsigned int lSocketAddressLen;
    struct sockaddr_in lSocketAddress;
    std::string response;
    
    // methods
    int startServer();
    int stopServer();
    void acceptConnection(int &socket);
    void sendResponse();
    std::string buildResponse();
};
}; //namespace http

#endif /* tcp_server_hpp */
