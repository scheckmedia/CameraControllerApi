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
        
    static GPContextErrorFunc _error_callback(GPContext *context, const char *text, void *data);
    static GPContextMessageFunc _message_callback(GPContext *context, const char *text, void *data);
        
    public:
        bool camera_found();
        bool is_initialized();
        
        static CameraController* getInstance();
        static void release();
        
        void init();
        bool is_busy();
        void is_bussy(bool busy);
        int capture(const char *filename, string &data);
        int preview(const char **file_data);
        int bulb(const char *filename, string &data);
        int get_settings(ptree &sett);
        int get_settings_value(const char *key, string &val);
        int get_settings_choices(const char *key, std::vector<string> &choices);
        int set_settings_value(const char *key, const char *val);
        int get_files(ptree &tree);
        int get_file(const char *filename, const char *filepath, string &base64out);
                
    private:
        static CameraController *_instance;        
        Camera *_camera;
        GPContext *_ctx;
        bool _is_busy;
        bool _liveview_running;
        bool _camera_found;
        bool _is_initialized;
        bool _save_images;
        
        CameraController();
        ~CameraController();
        
        void _init_camera();
        int _wait_and_handle_event (useconds_t waittime, CameraEventType *type, int download);
        int _get_files(ptree &tree, const char *folder);
        void _build_settings_tree(CameraWidget *w);
        void _read_widget(CameraWidget *w, ptree &tree, string node);
        void _get_item_value(CameraWidget *w, ptree &tree);
        void _set_capturetarget(int index);
        int _file_to_base64(CameraFile *file, string &output);
        int _file_to_base64(const char *data, unsigned int data_size, string &output);
    };
}

#endif /* defined(__CameraControllerApi__CameraController__) */
