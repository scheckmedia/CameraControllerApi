//
//  main.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 09.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include <iostream>
#include "Settings.h"
#include "Server.h"
#include "CameraController.h"
#include "Api.h"

using namespace CameraControllerApi;
Server *srv;

void sighandler(int sig){
    srv->terminate(sig);
}

int main(int argc, const char * argv[])
{
    if(argc != 2){
        printf("%s PORT\n", argv[0]);
        exit(1);
    }

    try {
        string port;
        Settings *cfg = Settings::getInstance();
        bool ret = cfg->get_value("server.port", port);
        
        if(ret){
            int http_port = atoi(port.c_str());
            srv = new Server(http_port);
            signal(SIGTERM, sighandler);
        }
    } catch (std::exception const &e) {
        std::cout<<"Error: " << e.what();
    }
    
    
    return 0;
}

