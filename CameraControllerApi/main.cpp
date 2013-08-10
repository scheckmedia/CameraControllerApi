//
//  main.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 09.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include <iostream>
#include "Server.h"

using namespace CameraControllerApi;

Server *srv;

void sighandler(int sig){
    srv->terminate(sig);
}

int main(int argc, const char * argv[])
{
    std::cout << "test";
    printf("Nu");
    if(argc != 2){
        printf("%s PORT\n", argv[0]);
        exit(1);
    }
    
    signal(SIGTERM, sighandler);
    
    int http_port = atoi(argv[1]);
    srv = new Server(http_port);
    return 0;
}

