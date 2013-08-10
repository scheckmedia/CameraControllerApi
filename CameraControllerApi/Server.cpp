//
//  Server.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 09.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include <pthread.h>
#include "Server.h"
#include <map>
#include <string>
#include "Command.h"

using std::map;
using std::string;
using namespace CameraControllerApi;

#define PAGE "<html><head><title>Error</title></head><body></body></html>"


Server::Server(int port){
    this->_port = port;
    this->_shoulNotExit = 1;
    
    pthread_t tServer;
    if (0 != pthread_create(&tServer, NULL, Server::initial, this)) {
        exit(0);
    }
    
    pthread_join(tServer, NULL);
}

void *Server::initial(void *context){
    Server *s = (Server *)context;
    s->http();
    
    return 0;
}

void Server::terminate(int sig){
    this->_shoulNotExit = 0;
}

int Server::send_bad_response( struct MHD_Connection *connection)
{
    static char *bad_response = (char *)PAGE;
    int bad_response_len = static_cast< int >(strlen(bad_response));
    int ret;
    struct MHD_Response *response;
    
    response = MHD_create_response_from_buffer ( bad_response_len,
                                                bad_response,MHD_RESPMEM_PERSISTENT);
    if (response == 0){
        return MHD_NO;
    }
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);
    return ret;
}


int Server::get_url_args(void *cls, MHD_ValueKind kind, const char *key , const char* value){
    map<string, string> *args = static_cast<map<string,string>*>(cls);
    if(args->find(key) == args->end()){
        if(!value){
            (*args)[key] = "";
        } else {
            (*args)[key] = value;
        }
    }   
        
    return MHD_YES;
}

int Server::url_handler (void *cls,
                        struct MHD_Connection *connection,
                        const char *url,
                        const char *method,
                        const char *version,
                        const char *upload_data, size_t *upload_data_size, void **ptr){
    printf("connection received %s", method);
    int ret;
    map<string, string> url_args;
    map<string, string>:: iterator  it;

    string respdata;
    
    static int aptr;
    char *me;
    const char *val;
    const char *typexml = "xml";
    const char *typejson = "json";
    const char *type = typejson;
    
    CameraControllerApi::Command cmd;
    
    struct MHD_Response *response;
    
    if (0 != strcmp(method, "GET")) {
        return MHD_NO;
    }
    
    if(&aptr != *ptr){
        *ptr = &aptr;
        return MHD_YES;
    }
    
    if(MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, Server::get_url_args, &url_args) < 0){
        return Server::send_bad_response(connection);
    }
    
    cmd.execute(url, url_args, respdata);
    
    
    *ptr = 0;
    //val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "q");
    me = (char *)malloc(respdata.size() + 1);
    if(me == 0)
        return MHD_NO;
    strncpy(me, respdata.c_str(), respdata.size() + 1);    

    response = MHD_create_response_from_buffer(strlen(me), (void *)me, MHD_RESPMEM_MUST_COPY);
    
    if(response == 0){
        free(me);
        return MHD_NO;
    }
    
    it = url_args.find("type");
    if (it != url_args.end() && strcasecmp(it->second.c_str(), "xml")) {
        type = typexml;
    }
    
    if(type == typejson)
        MHD_add_response_header(response, "Content-Type", "application/json");
    else {        
        MHD_add_response_header(response, "Content-Type", "application/xml");
    }
    MHD_add_response_header(response, "Content-Disposition", "attachment;filename=\"cca.json\"");
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

void *Server::http(){
    printf("Port %d", this->_port);
    printf("Huhu");
    struct MHD_Daemon *d;
    d = MHD_start_daemon(MHD_USE_DEBUG|MHD_USE_SELECT_INTERNALLY|MHD_USE_POLL, this->_port,
                         0, 0, this->url_handler, (void*)PAGE ,MHD_OPTION_END);
    if(d==0){
        return 0;
    }
    
    while (this->_shoulNotExit) {
        sleep(1);
    }

    
    MHD_stop_daemon(d);    
    return 0;
}
