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
    
    class CameraController {    
        
    static void* start_liveview_server(void *context);
    static GPContextErrorFunc _error_callback(GPContext *context, const char *text, void *data);
    static GPContextMessageFunc _message_callback(GPContext *context, const char *text, void *data);
        
    public:
        bool camera_found();
        bool is_initialized();
        
        static CameraController* getInstance();
        static void release();
        
        int capture(const char *filename, string &data);
        int preview(const char **file_data);
        int liveview_start();
        int liveview_stop();
        int trigger();
        int get_settings(ptree &sett);
        int get_settings_value(const char *key, void *val);
        int set_settings_value(const char *key, const char *val);
                
    private:
        static CameraController *_instance;        
        Camera *_camera;
        GPContext *_ctx;
        bool _running_process;
        bool _camera_found;
        bool _is_initialized;
        
        CameraController();
        ~CameraController();
        
        void _init_camera();
        int _wait_event_and_download (Camera *camera, int waittime, GPContext *context);
        
        
        void _build_settings_tree(CameraWidget *w);
        void _read_widget(CameraWidget *w, ptree &tree, string node);
        void _get_item_value(CameraWidget *w, ptree &tree);                
    };
}

#endif /* defined(__CameraControllerApi__CameraController__) */
