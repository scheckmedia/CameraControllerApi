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
    set<string>* params;
};

Command::Command(Api *api){
    this->_api = api;
    set<string> params;
    string param_camera_settings[] = {"list", "aperture", "speed", "iso", "white_balance","focus_point","focus_mode"};
    string param_execute[] = {"shot","time_lapse","burst"};
    _valid_commands["/settings"] = set<string>(param_camera_settings, param_camera_settings + 6);
    _valid_commands["/capture"] = set<string>(param_execute, param_execute + 3);
}

int Command::execute(const string &url, const map<string, string> &argvals, string &response){
    CCA_API_RESPONSE resp;
    CCA_API_OUTPUT_TYPE type = CCA_OUTPUT_TYPE_JSON;
    validate_data vdata;
    vdata.api = url;
    vector<string> params;
    set<string> uniqueparams;
    map<string,string>::const_iterator iterator = argvals.find("action");
    
    if(iterator != argvals.end()){
        string param = iterator->second;
        boost::trim(param);
        boost::split(params, param, boost::is_any_of(","));
    }
    
    BOOST_FOREACH(string pr, params ) {
        uniqueparams.insert(pr);
    }
    
    iterator = argvals.find("type");
    if(iterator != argvals.end()){
        const string out_type = iterator->second;
        if(strcasecmp(out_type.c_str(), "xml") == 0)
            type = CCA_OUTPUT_TYPE_XML;
    }

    vdata.params = &uniqueparams;
        
    if ( !this->_validate(&vdata) || uniqueparams.size() < 1) {
        resp = CCA_API_RESPONSE_INVALID;
        ptree p;
        Api::buildResponse(p, type, resp, response);
        return resp;
    }    
    
    return this->_executeAPI(url, uniqueparams, argvals, type, response);
}

bool Command::_executeAPI(const string &url, const set<string> &actions, const map<string, string> &urlparams, CCA_API_OUTPUT_TYPE type, string &response){
    bool ret = CCA_CMD_SUCCESS;
    
    if(url == "/settings"){
        if(actions.find("list") != actions.end()){
            ret = this->_api->list_settings(type, response);
        } else if(actions.find("focus_point") != actions.end()){
            
            map<string,string>::const_iterator iterator = urlparams.find("point");
            if(iterator == urlparams.end()){
                return false;
            }
            
            string param = iterator->second;
            boost::trim(param);
            ret = this->_api->set_focus_point(param, type, response);
        }
    } else if(url == "/capture"){
        if(actions.find("shot") != actions.end()){
            ret = this->_api->shot(type, response);
        } else if(actions.find("burst") != actions.end()){
            map<string,string>::const_iterator iterator = urlparams.find("photos");
            if(iterator == urlparams.end()){
                return false;
            }
            
            string param = iterator->second;
            boost::trim(param);
            ret = this->_api->burst(atoi(param.c_str()), type, response);
        }
    }
    return ret;
}

bool Command::_validate(const void *data){
    const validate_data *vdata = static_cast<const validate_data *>(data);
    map<string, set<string>>::iterator it = this->_valid_commands.find(vdata->api);
    
    if(it == this->_valid_commands.end()){
        return false;
    }
    
    set<string>::iterator param_it = vdata->params->begin();
    while(param_it != vdata->params->end()){
        if(it->second.find(*param_it) == it->second.end())
            return false;
        
        ++param_it;
    }
    
    return true;
}

void Command::_getInvalidResponse(string& response){
    response = "Some error in your data ";
}
