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
#include <sstream>


namespace CameraControllerApi {
    class Server{
        static int get_url_args(void *cls, MHD_ValueKind kind, const char *key , const char* value);
        static int send_bad_response( struct MHD_Connection *connection);
        static int send_auth_fail( struct MHD_Connection *connection);
        
    public:
        static Server* getInstance();
        void run(int port);
        Api *api;
        CameraController *cc;
        Command *cmd;
        
        static void* initial(void*);
        
        void terminate(int sig);
        void http();
        void http_mjpeg();
        
        
    private:
        Server();
        static Server *_instance;
        static int url_handler (void *cls,
                                struct MHD_Connection *connection,
                                const char *url,
                                const char *method,
                                const char *version,
                                const char *upload_data, size_t *upload_data_size, void **ptr);
        static MHD_Response* handle_webif(void *cls,
									   struct MHD_Connection *connection,
									   const char *url);
        static MHD_Response* handle_api(void *cls,
									   const char *url,
									   map<string, string> url_args,
									   string respdata,
									   void **ptr);
        static ssize_t handle_mjpeg(void *cls, uint64_t pos, char *buf, size_t max);
        static void free_mjpeg(void *cls);
                
        
        int _port;
        int _shoulNotExit;
        bool _webif;
        bool _auth;
        std::stringstream *_live_stream;
        
    };
}
#endif /* defined(__CameraControllerApi__Server__) */
