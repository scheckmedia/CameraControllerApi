//
//  Server.h
//  CameraControllerApi
//
//  Created by Tobias Scheck on 09.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#ifndef __CameraControllerApi__Server__
#define __CameraControllerApi__Server__

#include <iostream>
#include "microhttpd.h"
#include "Api.h"
#include "CameraController.h"
#include "Command.h"

namespace CameraControllerApi {
    class Server{
        
        static int get_url_args(void *cls, MHD_ValueKind kind, const char *key , const char* value);
        static int send_bad_response( struct MHD_Connection *connection);
        static int send_auth_fail( struct MHD_Connection *connection);
        
    public:
        Server(int port);
        Api *api;
        CameraController *cc;
        Command *cmd;
        
        static void* initial(void*);
        
        void terminate(int sig);
        void *http();
        
        
    private:
        static int url_handler (void *cls,
                                struct MHD_Connection *connection,
                                const char *url,
                                const char *method,
                                const char *version,
                                const char *upload_data, size_t *upload_data_size, void **ptr);
                
        
        int _port;
        int _shoulNotExit;
        bool _webif;
        bool _auth;
        
    };
}
#endif /* defined(__CameraControllerApi__Server__) */
