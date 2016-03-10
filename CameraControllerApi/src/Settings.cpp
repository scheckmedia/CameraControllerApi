//
//  Settings.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 28.09.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include "Settings.h"

using namespace CameraControllerApi;


Settings* Settings::_instance = NULL;

Settings* Settings::getInstance(){
    if(_instance == NULL){
        _instance = new Settings();
    }

    return _instance;
}

void Settings::release(){
    if(_instance != NULL)
        delete _instance;

    _instance = NULL;
}

Settings::Settings() { }

void Settings::_init() {
    if(this->_base_path.empty())
        throw std::runtime_error("no base path available");

    if(this->_initialized == false) {
        boost::property_tree::read_xml(this->_base_path + "/" + CCA_SETTINGS_FILE, _pt);
        this->_initialized = true;
    }
}

bool Settings::get_value(string key, string &res){
    if(this->_initialized == false) {
        this->_init();
    }

    try {
        res = _pt.get<string>("CCA_SETTINGS."+key);
        return true;
    } catch (std::exception const &e) {
        std::cout<<"Error: " << e.what();
    }
    return false;
}

string Settings::get_value(string key){
    string res = "";
    Settings *s = Settings::getInstance();
    s->get_value(key, res);
    return res;
}

void Settings::base_path(string path) {
    this->_base_path = path;
}

string Settings::get_base_path() {
    return this->_base_path;
}
