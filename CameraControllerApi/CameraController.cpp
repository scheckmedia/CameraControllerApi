//
//  CameraController.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 10.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include "CameraController.h"
#include <vector>

using namespace CameraControllerApi;
using std::exception;
using std::string;
using std::vector;
using boost::property_tree::ptree;

CameraController::CameraController(){
    if(!this->_camera_found){        
        gp_camera_new(&this->_camera);
        this->_ctx = gp_context_new();
        gp_context_set_error_func(this->_ctx, (GPContextErrorFunc)CameraController::_error_callback, NULL);
        gp_context_set_message_func(this->_ctx, (GPContextMessageFunc)CameraController::_message_callback, NULL);
        
        int ret = gp_camera_init(this->_camera, this->_ctx);
        if(ret < GP_OK){            
            gp_camera_free(this->_camera);
        } else {
            this->_camera_found = true;
        }
        this->_is_initialized = true;
    }
}

CameraController::~CameraController(){
    gp_camera_unref(this->_camera);
    gp_context_unref(this->_ctx);
}


bool CameraController::camera_found(){
    return this->_camera_found;
}

bool CameraController::is_initialized(){
    return this->_is_initialized;
}

int CameraController::get_settings(ptree &sett){
    CameraWidget *w, *children;
    int ret;
    ret = gp_camera_get_config(this->_camera, &w, this->_ctx);
    if(ret < GP_OK){
        return false;
    }
    
    ret = gp_widget_get_child_by_name(w, "main", &children);
    if(ret < GP_OK)
        return false;

    this->_read_widget(children, sett, "settings");
    
    return true;
}



void CameraController::_read_widget(CameraWidget *w,  ptree &tree, string node){
    const char  *name;
    ptree subtree;
    gp_widget_get_name(w, &name);
    string nodename = node + "." + name;
    
    int items = gp_widget_count_children(w);
    if(items > 0){
        for(int i = 0; i < items; i++){
            CameraWidget *item_widget;
            gp_widget_get_child(w, i, &item_widget);
        
            this->_read_widget(item_widget, subtree, nodename);
        }
    } else {
        this->_get_item_value(w, subtree);
    }
    
    tree.put_child(name, subtree);
}

int CameraController::get_settings_value(const char *key, const char *val){
    CameraWidget *w, *child;
    CameraWidgetType type;
    int ret;
    
    ret = gp_camera_get_config(this->_camera, &w, this->_ctx);
    if(ret < GP_OK){
        
    }
    return true;
}

int CameraController::set_settings_value(const char *key, const char *val){

    CameraWidget *w, *focusWidget;
    int ret = gp_camera_get_config(this->_camera, &w, this->_ctx);
    if(ret < GP_OK)
        return false;
    
    ret = gp_widget_get_child_by_name(w, key, &focusWidget);
    if(ret < GP_OK)
        return false;
    
    ret = gp_widget_set_value(focusWidget, val);
    if(ret < GP_OK)
        return false;
    
    
    ret = gp_camera_set_config(this->_camera, w, this->_ctx);
    
    gp_widget_free(w);
    return (ret == GP_OK);
}

void CameraController::_get_item_value(CameraWidget *w, ptree &tree){
    CameraWidgetType type;
    int id;
    const char *item_label, *item_name;
    ptree item_choices;
    void* item_value;
    gp_widget_get_id(w, &id);
    gp_widget_get_label(w, &item_label);
    gp_widget_get_name(w, &item_name);
    gp_widget_get_value(w, &item_value);
    gp_widget_get_type(w, &type);
    
    int number_of_choices = gp_widget_count_choices(w);
    if(number_of_choices > 0){
        ptree choice_value;
        for(int i = 0; i < number_of_choices; i++){
            const char *choice;
            gp_widget_get_choice(w, i, &choice);
            choice_value.put_value(choice);
            item_choices.push_back(std::make_pair("", choice_value));
        }
    }
    
    

    tree.put("id",              id);
    tree.put("name",            item_name);
    tree.put("label",           item_label);
    tree.put_child("choices",   item_choices);
    
    switch (type) {
        case GP_WIDGET_TEXT:
        case GP_WIDGET_RADIO:
        case GP_WIDGET_MENU: {
            unsigned char *val = static_cast<unsigned char *>(item_value);
            tree.put("value", val);
            break;
        }
        case GP_WIDGET_TOGGLE:{
            //FIXME: problem with casting that value
            /*
             int *val = (int *)item_value;
             if(val)
             tree("value", *val);*/
            break;
        }
            
        case GP_WIDGET_RANGE: {
            //FIXME: problem with casting that value
            /*float *val = (float *)item_value;
             if(val)
             tree("value", *val);*/
            break;
        }
            
        default:
            break;
    }
}

int CameraController::_validate_widget(CameraWidget *w, const char *key, CameraWidget *child){
    return true;
}

GPContextErrorFunc CameraController::_error_callback(GPContext *context, const char *text, void *data){
    
    return 0;
}

GPContextMessageFunc CameraController::_message_callback(GPContext *context, const char *text, void *data){
    
    return 0;
}