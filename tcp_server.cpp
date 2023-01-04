//
//  tcp_server.cpp
//  web_server
//
//  Created by Сарапулов Михаил on 27.12.2022.
//

#include "tcp_server.hpp"

#include <iostream>
#include <sstream>
#include <unistd.h>

#define TRUE  1
#define FASLE 0

namespace
{
const long BUFFER_SIZE = 30720;
const std::string INFO = "INFO";
const std::string ERROR = "ERROR";
const int maxClient = 30;

void log(const std::string &type, const std::string &message)
{
    std::cout << "[" << type << "]" << " : " << message << std::endl;
};

void exitWithError(const std::string &errorMessage, int code)
{
    log(ERROR, errorMessage);
    exit(code);
}
};

namespace http
{
TcpServer::TcpServer(std::string ipAddress, int port): lIpAddress(ipAddress), lPort(port), lSocket(), incomingSocket(), lSocketAddressLen(sizeof(lSocketAddress)), response(buildResponse())
{
    lSocketAddress.sin_family = AF_INET;
    lSocketAddress.sin_port = htons(lPort);
    lSocketAddress.sin_addr.s_addr = inet_addr(lIpAddress.c_str());
    
    if (startServer() != 0)
    {
        std::ostringstream ss;
        ss << "Failed to start server with PORT: " << ntohs(lSocketAddress.sin_port);
        log(ERROR, ss.str());
    }
};

TcpServer::~TcpServer()
{
    stopServer();
}

int TcpServer::startServer()
{
    lSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (lSocket < 0)
    {
        int code = 1;
        exitWithError("Cannot creaete a socket", code);
        return code;
    }
    
    if (bind(lSocket, (sockaddr *)&lSocketAddress, lSocketAddressLen) < 0)
    {
        int code = 2;
        exitWithError("Cannot binding the socket to address", code);
        return code;
    }
    
    return lSocket;
}

void TcpServer::startListening()
{
    int opt = TRUE;
    char buffer[1024];
    int req, valread;

    if (listen(lSocket, 20) < 0)
    {
        exitWithError("Cannpt start listening", 3);
    }
    
    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: "
        << inet_ntoa(lSocketAddress.sin_addr)
        << " PORT: " << ntohs(lSocketAddress.sin_port)
        << " ***\n\n";
    log(INFO, ss.str());
    
    
    for (int i = 0; i <= maxClient; i++)
    {
        clientSocket[i] = 0;
    }
    
    if (setsockopt(lSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0)
    {
        int code = 6;
        exitWithError("Set socket opt fail", code);
    }
    
    
    while (true) {
        log(INFO, "====== Waiting for a new connection ======\n\n\n");

        FD_ZERO(&readFds);
        
        FD_SET(lSocket, &readFds);
        maxSd = lSocket;
        
        for (int i = 0 ; i < maxClient ; i++)
        {
            //socket descriptor
            sd = clientSocket[i];
                 
            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET(sd , &readFds);
                 
            //highest file descriptor number, need it for the select function
            if(sd > maxSd)
                maxSd = sd;
        }
        
        activity = select(maxSd + 1, &readFds, NULL, NULL, NULL);
        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }
        
        if(FD_ISSET(lSocket, &readFds))
        {
            acceptConnection(incomingSocket);
            
            char buffer[BUFFER_SIZE] = {0};
            req = read(incomingSocket, buffer, BUFFER_SIZE);
            if (req < 0)
            {
                int code = 5;
                exitWithError("Failed to receive bytes from client socket connection", code);
            }
            
            std::ostringstream ss;
                        ss << "------ Received Request from client ------\n\n";
            log(INFO, ss.str());
            
            sendResponse();
            
            close(incomingSocket);
        }
        
        for (int i = 0; i < maxClient; i++)
        {
            sd = clientSocket[i];
                 
            if (FD_ISSET( sd, &readFds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&lSocketAddress , \
                        (socklen_t*)&lSocketAddressLen);
                    printf("Host disconnected , ip %s , port %d \n" ,
                          inet_ntoa(lSocketAddress.sin_addr) , ntohs(lSocketAddress.sin_port));
                         
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    clientSocket[i] = 0;
                }
                     
                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end
                    //of the data read
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
}

void TcpServer::sendResponse()
{
    long byteSent;
    
    byteSent = write(incomingSocket, response.c_str(), response.size());
    
    if (byteSent == response.size())
    {
        log(INFO, "------ Server Response sent to client ------\n\n");
    } else
    {
        log(ERROR ,"Error sending response to client");
    }
    
}

void TcpServer::acceptConnection(int &socket)
{
    socket = accept(lSocket, (sockaddr *)&lSocketAddress, &lSocketAddressLen);
    
    if (socket < 0)
    {
        int code = 4;
        std::ostringstream ss;
                ss <<
                "Server failed to accept incoming connection from ADDRESS: "
                << inet_ntoa(lSocketAddress.sin_addr) << "; PORT: "
                << ntohs(lSocketAddress.sin_port);
                exitWithError(ss.str(), code);
    }
}

std::string TcpServer::buildResponse()
{
    std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
    std::ostringstream ss;
    ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
       << htmlFile;

    return ss.str();
}

int TcpServer::stopServer()
{
    close(lSocket);
    close(incomingSocket);
    exit(0);
}
}; //namespace http
