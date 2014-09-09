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

Settings::Settings(){
    boost::property_tree::read_xml(CCA_SETTINGS_FILE, _pt);
}

bool Settings::get_value(string key, string &res){
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
