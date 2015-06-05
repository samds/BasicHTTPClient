//
//  asio_client.h
//  BasicHTTPClient
//
//  Created by samuel.
//  Copyright (c) 2014 SDS. All rights reserved.
//

#ifndef BasicHTTPClient_asio_client_h
#define BasicHTTPClient_asio_client_h

#define ASIO_STANDALONE
#include "asio.hpp"

bool ASIO_init_http_client()
{
    asio::ip::tcp::iostream s("www.example.com", "http");
    s.expires_from_now(std::chrono::seconds(60));
    s << "GET / HTTP/1.1\r\n";
    s << "Host: www.example.com\r\n";
    s << "\r\n";
    std::string header;
    while (std::getline(s, header) && header != "\r")
        std::cout << header << "\n";
    std::cout << s.rdbuf();
    
    if (!s) {
        std::cout << "Socket error: " << s.error().message() << "\n";
        return false;
    }
    
    return true;
}

#endif
