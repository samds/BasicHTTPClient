//
//  posix_client.h
//  BasicHTTPClient
//
//  Created by samuel.
//  Copyright (c) 2014 SDS. All rights reserved.
//

#ifndef BasicHTTPClient_posix_client_h
#define BasicHTTPClient_posix_client_h

#include <arpa/inet.h>      // inet_ntop()
#include <errno.h>          // errno
#include <netdb.h>          // addrinfo
#include <netinet/in.h>     // sockaddr_in
#include <stdbool.h>        // bool, true, false
#include <stdio.h>          // perror(), printf()
#include <stdlib.h>         // exit, EXIT_FAILURE
#include <string.h>         // memset(), strerror()
#include <sys/socket.h>     // AF_INET, SOCK_STREAM,
#include <unistd.h>         // close()

#define INVALID_SOCKET      (-1)
#define SOCKET_ERROR        (-1)
#define SEND_ERROR          (-1)
#define HOST                "www.example.com"
#define PORT                "http"
#define GETADDRINFO_SUCCEED (0)
#define MSG_NOFLAG          (0x00)

// socket type
typedef int         socket_t;

bool POSIX_init_http_client()
{
    int status;
    struct addrinfo hints, *res, *it;
    int sockfd; // socket file descriptor

    // Initialises the struct with 0
    memset(&hints, 0, sizeof hints);
    
    // first, load up address structs with getaddrinfo():
    
    // AF_UNSPEC make it IP version-agnostic.
    // Can use IPv4(AF_INET) or IPv6(AF_INET6), whichever.
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    
    // We use static host and port but can be dynamic
    const char *hostname = HOST;
    const char *servname = PORT;
    
    status = getaddrinfo(hostname, servname, &hints, &res);
    if(status != GETADDRINFO_SUCCEED) {
        fprintf(stderr, "getaddrinfo failed, reason: %s\n",gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    
    // browses all addrinfo associated to the given host and service name
    // and try to connect to the first one working.
    for (it = res; it != NULL; it = it->ai_next) {
        
        // make a socket:
        sockfd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        
        if (sockfd == SOCKET_ERROR) {
            fprintf(stderr, "socket failed, reason: %s\n",strerror(errno));
            continue;
        }
        
        // We don't call bind() !!
        // Basically, we don't care about our local port number; we only care where
        // we're going (the remote port). The kernel will choose a local port for
        // us, and the site we connect to will automatically get this information
        // from us. No worries.
        //
        //
        // bind it to the port we passed in to getaddrinfo():
        // bind(sockfd, res->ai_addr, res->ai_addrlen);
        //
        // int yes = 1;
        // if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&yes, sizeof(int)) == -1)
        // {
        //     perror("setsockopt");
        //     exit(EXIT_FAILURE);
        // }
        
        if(connect(sockfd, res->ai_addr, res->ai_addrlen) == SOCKET_ERROR)
        {
            perror("connect()");
            continue;
        }
        
        //        const char get_request[] = "GET / HTTP/1.1\r\n" "Host: www.example.com\r\n" "\r\n";
        char get_request[1024];
        sprintf(get_request, "GET / HTTP/1.1\r\nHost:%s\r\n\r\n", hostname);
        int flags = 0; // no flags
        size_t length = sizeof(get_request);
        ssize_t bytes_cnt = send(sockfd, (const void *)get_request, length, flags);
        if (bytes_cnt > 0) {
            
        }
        else if (bytes_cnt == 0) {
            printf("the remote side has closed the connection on you!");
        }
        else {
            perror("send()");
        }
      
        // Reads the file
        char buffer[1024];
        flags = 0; // no flags
        length = sizeof(buffer);
        ssize_t bytes_recv = 1;
        while (bytes_recv!=0) {
            bytes_recv = recv(sockfd, (void *)buffer, length, flags);
            if (bytes_recv > 0) {
                fprintf(stdout, "%.*s", (int)bytes_recv, buffer);
            }
            else if (bytes_recv == 0) {
                printf("the remote side has closed the connection on you!\n");
            }
            else {
                perror("recv()");
            }
        }
        
        printf("\n");
        
        close(sockfd);
        break;
    }
    
    freeaddrinfo(res);
    return true;
}

#endif
