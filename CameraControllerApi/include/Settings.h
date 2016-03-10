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

#define CCA_SETTINGS_FILE "settings.xml"

namespace CameraControllerApi {
    using std::string;
    using boost::property_tree::ptree;
    using boost::property_tree::basic_ptree;

    class Settings {
        static Settings *_instance;
    public:
        static Settings* getInstance();
        static string get_value(string key);
        static void release();
        bool get_value(string key, string &res);
        void base_path(string path);
        string get_base_path();

    private:
        Settings();
        ~Settings(){};
        void _init();
        bool _initialized;
        string _base_path;
        ptree _pt;

    };
}

#endif /* defined(__CameraControllerApi__Settings__) */
