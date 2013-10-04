//
//  Settings.h
//  CameraControllerApi
//
//  Created by Tobias Scheck on 28.09.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#ifndef __CameraControllerApi__Settings__
#define __CameraControllerApi__Settings__

#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#define CCA_ERROR_SETTINGS_FILE "settings.xml"

namespace CameraControllerApi {
    using std::string;
    using boost::property_tree::ptree;
    using boost::property_tree::basic_ptree;
    
    class Settings {
    public:
        static Settings* getInstance();
        static void release();
        bool get_value(string key, string &res);
        
    private:
        Settings();
        ~Settings(){};
        ptree _pt;
        static Settings *_instance;
        
    };
}

#endif /* defined(__CameraControllerApi__Settings__) */
