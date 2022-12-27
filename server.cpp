//
//  main.cpp
//  web_server
//
//  Created by Сарапулов Михаил on 26.12.2022.
//

#include <iostream>
#include "tcp_server.hpp"

int main(int argc, const char * argv[]) {
    using namespace http;
    
    TcpServer server = TcpServer("0.0.0.0", 8800);
    server.startListening();
    
    return 0;
}
