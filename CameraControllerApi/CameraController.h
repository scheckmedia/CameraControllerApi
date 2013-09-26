//
//  CameraController.h
//  CameraControllerApi
//
//  Created by Tobias Scheck on 10.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#ifndef __CameraControllerApi__CameraController__
#define __CameraControllerApi__CameraController__

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <gphoto2/gphoto2-camera.h>
#include <boost/property_tree/ptree.hpp>


using std::string;
using boost::property_tree::ptree;

namespace CameraControllerApi {
    
    class CameraNotFound:public std::exception{
    public:
        CameraNotFound(const string m):msg(m){}
        const char* what(){return msg.c_str();}
    private:
        string msg;
    };
    
    class CameraController {     
    public:
        bool camera_found();
        bool is_initialized();
        CameraController();
        ~CameraController();
        int capture(const char *filename, string &data);
        int get_settings(ptree &sett);
        int get_settings_value(const char *key, void *val);
        int set_settings_value(const char *key, const char *val);
                
    private:
        Camera *_camera;
        GPContext *_ctx;
        bool _camera_found;
        bool _is_initialized;
        
        bool _toBase64(char* dest, const char* src, uint size);
        
        void _build_settings_tree(CameraWidget *w);
        void _read_widget(CameraWidget *w, ptree &tree, string node);
        void _get_item_value(CameraWidget *w, ptree &tree);
        int _validate_widget(CameraWidget *w, const char *key, CameraWidget *child);
        
        static GPContextErrorFunc _error_callback(GPContext *context, const char *text, void *data);
        static GPContextMessageFunc _message_callback(GPContext *context, const char *text, void *data);
    };
}

#endif /* defined(__CameraControllerApi__CameraController__) */
