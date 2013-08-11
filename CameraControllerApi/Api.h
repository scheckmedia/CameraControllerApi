//
//  Api.h
//  CameraControllerApi
//
//  Created by Tobias Scheck on 09.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#ifndef __CameraControllerApi__Api__
#define __CameraControllerApi__Api__

#include "CameraController.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#define CCA_ERROR_MESSAGE_FILE "/Users/tobi/Documents/c++/CameraControllerApi/CameraControllerApi/error_messages.xml"

namespace CameraControllerApi {
    
    using std::map;
    using std::string;
    using boost::property_tree::ptree;
    using boost::property_tree::basic_ptree;
    
    typedef enum {
        invalid = -1,
        success = 1
    } CCA_API_RESPONSE;
    
    typedef enum {
        CCA_OUTPUT_TYPE_XML,
        CCA_OUTPUT_TYPE_JSON
    } CCA_API_OUTPUT_TYPE;
    
    class Api {
    private:
        CameraController *_cc;
        
    public:
        Api(CameraController *cc);
        bool list_settings(string &output);
        
        static void buildResponse(void *data, CCA_API_OUTPUT_TYPE type, CCA_API_RESPONSE resp, string &output);
        static void errorMessage(CCA_API_RESPONSE errnr, string &message);
    };
}

#endif /* defined(__CameraControllerApi__Api__) */
