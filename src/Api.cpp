//
//  Api.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 09.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include "Api.h"
#include "Settings.h"
#include <boost/lexical_cast.hpp>

using namespace CameraControllerApi;

Api::Api(CameraController *cc){
    this->_cc = cc;
}

bool Api::status(CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        ptree settings;
        Api::buildResponse(settings, type, CCA_API_RESPONSE_SUCCESS, output);

        return true;
    }

    return false;
}

bool Api::init(CCA_API_OUTPUT_TYPE type, string &output){
    this->_cc->init();
    if(this->_check(type, output)){
        ptree settings;
        Api::buildResponse(settings, type, CCA_API_RESPONSE_SUCCESS, output);
        return true;
    }
    return false;
}

bool Api::list_settings(CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        ptree settings;
        this->_cc->get_settings(settings);
        Api::buildResponse(settings, type, CCA_API_RESPONSE_SUCCESS, output);
        return true;
    }

    return false;
}

bool Api::get_settings_by_key(string key, CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        ptree settings;
        string value;
        int ret;
        ret = this->_cc->get_settings_value(key.c_str(), value);

        if(ret < GP_OK)
            return ret;

        std::vector<string> choices;
        ret = this->_cc->get_settings_choices(key.c_str(), choices);

        if(ret < GP_OK)
            return ret;

        settings.put("value", value);

        ptree choices_items;
        for(int i = 0; i < choices.size(); i++){
            ptree choice_value;
            string choice = choices.at(i);

            choice_value.put_value(choice);
            choices_items.push_back(std::make_pair("", choice_value));
        }
        settings.put_child("choices", choices_items);
        Api::buildResponse(settings, type, CCA_API_RESPONSE_SUCCESS, output);
        return true;
    }

    return false;
}

bool Api::list_files(CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        ptree settings;
        int ret = this->_cc->get_files(settings);

        if(ret)
            Api::buildResponse(settings, type, CCA_API_RESPONSE_SUCCESS, output);
        else
            Api::buildResponse(settings, type, CCA_API_RESPONSE_INVALID, output);

        return true;
    }

    return false;
}

bool Api::get_file(string file, string path, CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        ptree tree;
        string image;
        int ret = this->_cc->get_file(file.c_str(), path.c_str(), image);

        if(ret){
            tree.put("image", image);
            Api::buildResponse(tree, type, CCA_API_RESPONSE_SUCCESS, output);

        } else {
            Api::buildResponse(tree, type, CCA_API_RESPONSE_INVALID, output);
        }

        return ret;
    }
    return false;
}

bool Api::set_focus_point(string focus_point, CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        return this->_set_settings_value("d108", focus_point, type, output);
    }
    return false;
}

bool Api::set_aperture(string aperture, CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        return this->_set_settings_value("f-number", aperture, type, output);
    }
    return false;
}

bool Api::set_speed(string speed, CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        return this->_set_settings_value("shutterspeed2", speed, type, output);
    }
    return false;
}

bool Api::set_iso(string iso, CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        return this->_set_settings_value("iso", iso, type, output);
    }
    return false;
}

bool Api::set_whitebalance(string wb, CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        return this->_set_settings_value("whitebalance", wb, type, output);
    }
    return false;
}

bool Api::shot(CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        ptree tree;
        string image;
        int ret = this->_cc->capture("image.jpg", image);
        if(ret){
            tree.put("image", image);
            Api::buildResponse(tree, type, CCA_API_RESPONSE_SUCCESS, output);

        } else {
            Api::buildResponse(tree, type, CCA_API_RESPONSE_INVALID, output);
        }

        return true;
    }
    return false;
}

bool Api::timelapse(int interval, time_t start, time_t end, CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        ptree tree;
        time_t now;
        time(&now);

        int ret;
        float shutterspeed;

        string val;
        ret = this->_cc->get_settings_value("shutterspeed", val);

        if(ret < GP_OK)
            return false;

        time_t diff = end - start;
        shutterspeed = std::stof(val);

        //buffer for exposure and image process time
        if (interval >= shutterspeed * 2 && (start + 1000) > now && diff > 0) {
            //delay
            this->_cc->is_bussy(true);
            while(start >= now){
                time(&now);
                usleep(100);
            }

            time_t timer = 0;
            time_t idiff = 0;
            time_t active = 0;
            while(this->_cc->is_busy() && diff > 0){

                if(idiff <= 0){
                    printf("shot at : %ld\n", diff);
                    ret = this->shot(type, output);
                    if(static_cast<int>(ret) < GP_OK)
                        break;

                    time(&timer);
                    timer += interval;
                }

                usleep(100);
                time(&active);
                diff  = end - active;
                idiff = timer - active;
            }
            this->_cc->is_bussy(false);
            Api::buildResponse(tree, type, CCA_API_RESPONSE_SUCCESS, output);
            return true;
        } else {
            this->_cc->is_bussy(false);
            Api::buildResponse(tree, type, CCA_API_RESPONSE_INVALID, output);
            return false;
        }
    }
    return false;
}

bool Api::bulb(CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        return this->_cc->bulb("bulb.jpg", output);
    }
    return false;
}

bool Api::burst(int number_of_images, CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        int ret = 0;
        ptree tree, images;
        string base64image;
        char filename[256];

        for(int i = 0; i < number_of_images; i++){

            snprintf(filename, 256, "shot-%04d.jpg", i);
            if(ret){
                ptree image;
                image.put_value(base64image);
                images.push_back(std::make_pair("", image));
            } else {
                return false;
            }
        }
        tree.put_child("preview_images", images);
        Api::buildResponse(tree, type, CCA_API_RESPONSE_SUCCESS, output);
        return true;
    }
    return false;
}

bool Api::autofocus(CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_check(type, output)){
        string value = "1";
        /*
         not needed
        this->_cc->get_settings_value("autofocusdrive", (void *)&value);
        if(value.empty()){
            int val = atoi(value.c_str());
            val++;
            value = boost::lexical_cast<string>(val);
        }*/
        this->_set_settings_value("autofocusdrive", value, type, output);
        return true;
    }
    return false;
}

bool Api::_buildCameraNotFound(CCA_API_RESPONSE resp, CCA_API_OUTPUT_TYPE type, string &output){
    ptree n;
    Api::buildResponse(n, type, resp, output);
    return false;
}

void Api::buildResponse(ptree data, CCA_API_OUTPUT_TYPE type, CCA_API_RESPONSE resp, string &output){
    try{
        boost::property_tree::ptree root;
        std::stringstream ss;
        if(resp != CCA_API_RESPONSE_SUCCESS){
            root.put("cca_response.state", "fail");
            string message;
            Api::errorMessage(resp, message);
            root.put("cca_response.message", message);
        } else {
            root.put("cca_response.state", "success");
            string message;
            if(data.empty() == false)
                root.add_child("cca_response.data", data);
        }

        if(type == CCA_OUTPUT_TYPE_JSON){
            boost::property_tree::write_json(ss, root);
        } else if(type == CCA_OUTPUT_TYPE_XML){
            boost::property_tree::write_xml(ss, root);
        }

        output = ss.str();
    } catch(std::exception &e){
        std::cout<<"Error: " << e.what();
    }
}

bool Api::_set_settings_value(string key, string value, CCA_API_OUTPUT_TYPE type, string &output){
    ptree tree;
    int ret = this->_cc->set_settings_value(key.c_str(), value.c_str());
    if(ret)
        Api::buildResponse(tree, type, CCA_API_RESPONSE_SUCCESS, output);
    else
        Api::buildResponse(tree, type, CCA_API_RESPONSE_INVALID, output);

    return ret;
}

bool Api::_check(CCA_API_OUTPUT_TYPE type, string &output){
    if(this->_cc->camera_found() == false)
        return this->_buildCameraNotFound(CCA_API_RESPONSE_CAMERA_NOT_FOUND, type, output);
    else if(this->_cc->is_busy()){
        ptree n;
        Api::buildResponse(n, type, CCA_API_RESPONSE_CAMERA_BUSY, output);
        return false;
    }
    return true;
}

void Api::errorMessage(CCA_API_RESPONSE errnr, string &message){
    try {
        boost::property_tree::ptree pt;
        Settings *s = Settings::getInstance();
        boost::property_tree::read_xml( s->get_base_path() + "/" + CCA_ERROR_MESSAGE_FILE, pt);
        std::ostringstream errorstr;
        errorstr << errnr;

        string error_id = errorstr.str();

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