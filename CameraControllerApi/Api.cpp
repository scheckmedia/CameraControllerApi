//
//  Api.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 09.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include "Api.h"



using namespace CameraControllerApi;

Api::Api(CameraController *cc){
    this->_cc = cc;
}

bool Api::list_settings(string &output){
   
    return true;
}

void Api::buildResponse(void *data, CCA_API_OUTPUT_TYPE type, CCA_API_RESPONSE resp, string &output){
    try{
        boost::property_tree::ptree root;
        std::stringstream ss;
        if(resp == CCA_API_RESPONSE::invalid){
            root.put("cca_response.state", "fail");
            string message;
            Api::errorMessage(resp, message);
            root.put("cca_response.message", message);
        }
        
        if(type == CCA_OUTPUT_TYPE_JSON){
            boost::property_tree::write_json(ss, root);
        }
        
        output = ss.str();
    } catch(std::exception &e){
        std::cout<<"Error: " << e.what();
    }
    
}

void Api::errorMessage(CCA_API_RESPONSE errnr, string &message){
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::read_xml(CCA_ERROR_MESSAGE_FILE, pt);
        string error_id = std::to_string(errnr);

        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("CCA.errors")){
            std::string id = v.second.get_child("<xmlattr>.id").data();
            if(id == error_id){
                message = v.second.data();
                return;
            }
        }
        message = pt.get<std::string>("CCA.errors", 0);
    } catch (std::exception const &e) {
        std::cout<<"Error: " << e.what();
    }
}