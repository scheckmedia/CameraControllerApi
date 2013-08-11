//
//  CameraController.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 10.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include "CameraController.h"

using namespace CameraControllerApi;
using std::exception;
using std::string;

CameraController::CameraController(){
    if(!this->_camera_found){
        gp_camera_new(&this->_camera);
        this->_ctx = gp_context_new();
        gp_context_set_error_func(this->_ctx, (GPContextErrorFunc)CameraController::_error_callback, NULL);
        gp_context_set_message_func(this->_ctx, (GPContextMessageFunc)CameraController::_message_callback, NULL);
        
        int ret = gp_camera_init(this->_camera, this->_ctx);
        if(ret < GP_OK){
            this->_camera_found = false;
            throw "Camera not found";
        }
        
        this->_camera_found = true;
    }
}

CameraController::~CameraController(){
    gp_camera_unref(this->_camera);
    gp_context_unref(this->_ctx);
}

bool CameraController::camera_found(){
    return this->_camera_found;
}

int CameraController::get_settings_value(const char *key, const char &val){
    CameraWidget *w, *child;
    CameraWidgetType type;
    int ret;
    
    ret = gp_camera_get_config(this->_camera, &w, this->_ctx);
    if(ret < GP_OK){
        
    }
    
}

int CameraController::set_settings_value(const char *key, const char &val){
    
}

int CameraController::_validate_widget(CameraWidget *w, const char *key, CameraWidget *child){
    
}

GPContextErrorFunc CameraController::_error_callback(GPContext *context, const char *text, void *data){
    
    return 0;
}

GPContextMessageFunc CameraController::_message_callback(GPContext *context, const char *text, void *data){
    
    return 0;
}