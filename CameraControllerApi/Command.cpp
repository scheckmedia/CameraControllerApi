//
//  Command.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 09.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include "Command.h"
#include "Api.h"
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>


using namespace CameraControllerApi;

struct validate_data {
    string api;
    string action;
};

Command::Command(Api *api){
    this->_api = api;
    set<string> params;
    string param_camera_settings[] = {"list", "aperture", "speed", "iso", "whitebalance","focus_point","focus_mode"};
    string param_execute[] = {"shot", "bulb", "time_lapse","autofocus", "manualfocus", "live"};
    string param_files[] = {"list", "get", "delete"};
    _valid_commands["/settings"] = set<string>(param_camera_settings, param_camera_settings + 7);
    _valid_commands["/capture"] = set<string>(param_execute, param_execute + 6);
    _valid_commands["/fs"] = set<string>(param_files, param_files + 3);
}

int Command::execute(const string &url, const map<string, string> &argvals, string &response){
    string param;
    CCA_API_OUTPUT_TYPE type = CCA_OUTPUT_TYPE_JSON;
    validate_data vdata;
    vdata.api = url;
    map<string,string>::const_iterator iterator = argvals.find("action");
    
    if(iterator != argvals.end()){
        param = iterator->second;
        boost::trim(param);
    }
    
    iterator = argvals.find("type");
    if(iterator != argvals.end()){
        const string out_type = iterator->second;
        if(strcasecmp(out_type.c_str(), "xml") == 0)
            type = CCA_OUTPUT_TYPE_XML;
    }

    vdata.action = param;
        
    if ( !this->_validate(&vdata) || param.empty()) {
        ptree p;
        CCA_API_RESPONSE ret = CCA_API_RESPONSE_INVALID;
        Api::buildResponse(p, type, ret, response);
        return ret;
    }    
    
    return this->_executeAPI(url, param, argvals, type, response);
}

bool Command::_executeAPI(const string &url, string action, const map<string, string> &urlparams, CCA_API_OUTPUT_TYPE type, string &response){
    bool ret = CCA_CMD_SUCCESS;

    
    string value;
    map<string,string>::const_iterator iterator = urlparams.find("value");
    
    if(iterator != urlparams.end()){
        value = iterator->second;
        boost::trim(value);
    }
    
    if(url == "/settings"){
        if(action.compare("list") == 0){
            ret = this->_api->list_settings(type, response);
        } else if(action.compare("focus_point") == 0){
            ret = this->_api->set_focus_point(value, type, response);
        } else if(action.compare("aperture") == 0){
            ret = this->_api->set_aperture(value, type, response);
        } else if(action.compare("speed") == 0){
            ret = this->_api->set_speed(value, type, response);
        } else if(action.compare("iso") == 0){
            ret = this->_api->set_iso(value, type, response);
        } else if(action.compare("whitebalance") == 0){
            ret = this->_api->set_whitebalance(value, type, response);
        }
        
    } else if(url == "/capture"){
        if(action.compare("shot") == 0){
           ret = this->_api->shot(type, response); 
        } else if(action.compare("live") == 0){
            if(value.compare("start") == 0)
                ret = this->_api->liveview(CCA_API_LIVEVIEW_START, type, response);
            else
                ret = this->_api->liveview(CCA_API_LIVEVIEW_STOP, type, response);
        } else if(action.compare("autofocus") == 0){
            ret = this->_api->autofocus(type, response);
        }
        
    } else if(url == "/fs"){
        if(action.compare("list") == 0){
            ret = this->_api->list_files(type, response);
        } else if(action.compare("get") == 0){
            map<string,string>::const_iterator iterator = urlparams.find("path");
            string path;
            
            if(iterator != urlparams.end()){
                path = iterator->second;
                boost::trim(value);
                ret = this->_api->get_file(value, path, type, response);
            } else {
                ret = false;
            }
        }
    }
    return ret;
}

bool Command::_validate(const void *data){
    const validate_data *vdata = static_cast<const validate_data *>(data);
    map<string, set<string> >::iterator it = this->_valid_commands.find(vdata->api);
    
    if(it == this->_valid_commands.end()){
        return false;
    }
    
    return true;
}
