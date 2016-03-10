//
//  main.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 09.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include "Settings.h"
#include "Server.h"
#include <boost/filesystem.hpp>
using namespace CameraControllerApi;
Server *srv;

void sighandler(int sig){
    if(srv != NULL)
        srv->terminate(sig);
}

int main(int argc, const char * argv[])
{
    try {
        string port;
        Settings *cfg = Settings::getInstance();
        std::string sub(argv[0]);
        cfg->base_path(sub.substr(0, sub.find_last_of("/")));
        bool ret = cfg->get_value("server.port", port);

        std::cout << "CameraControllerApi listening port " << port << std::endl;

        if(ret){
            int http_port = atoi(port.c_str());
            srv = new Server(http_port);
            signal(SIGTERM, sighandler);
        }

        cfg->release();
    } catch (std::exception const &e) {
        std::cout<<"Error: " << e.what();
    }


    return 0;
}

