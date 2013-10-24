//
//  CameraController.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 10.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include "CameraController.h"
#include "Settings.h"
#include "Helper.h"
#include "Base64.h"
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>

#include <boost/lexical_cast.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>


using namespace CameraControllerApi;
using namespace boost::system;
using namespace boost::asio;
using namespace std;

using boost::property_tree::ptree;



CameraController* CameraController::_instance = NULL;

CameraController* CameraController::getInstance(){
    if(_instance == NULL)
        _instance = new CameraController();
    
    
    return _instance;
}

void CameraController::release(){
    if(_instance != NULL){
        gp_camera_exit(_instance->_camera, _instance->_ctx);
        delete _instance;
    }
    
    _instance = NULL;
    
}


CameraController::CameraController(){
    this->init();
}

void CameraController::init(){
    if(!this->_camera_found){
        this->_init_camera();
        Settings *s = Settings::getInstance();
        string val = "";
        s->get_value("general.save_images", val);
        
        if(val.compare("TRUE") == 0 || val.compare("true") == 0)
            this->_save_images = true;
        else
            this->_save_images = false;
        
        
        if(this->_save_images && this->_camera_found)
            this->_set_capturetarget(1);
    }
}

void CameraController::_init_camera(){
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

CameraController::~CameraController(){
    gp_camera_exit(this->_camera, this->_ctx);
    gp_context_unref(this->_ctx);    
}


bool CameraController::camera_found(){
    return this->_camera_found;
}

bool CameraController::is_initialized(){
    return this->_is_initialized;
}

int CameraController::capture(const char *filename, string &data){
    int ret, fd ;
    CameraFile *file;
    CameraFilePath path;
       
    strcpy(path.folder, "/");
	strcpy(path.name, filename);
    
	ret = gp_camera_capture(this->_camera, GP_CAPTURE_IMAGE, &path, this->_ctx);
    if (ret != GP_OK)
        return ret;
    
    
    if(this->_save_images == false){
        ret = gp_file_new(&file);
    } else {
        fd  = open(filename, O_CREAT | O_RDWR, 0644);
        ret = gp_file_new_from_fd(&file, fd);
    }
    
    if (ret != GP_OK)
        return ret;
    
	ret = gp_camera_file_get(this->_camera, path.folder, path.name, GP_FILE_TYPE_NORMAL, file, this->_ctx);
    
    if (ret != GP_OK)
        return ret;
    
    this->_file_to_base64(file, data);
    
    if(this->_save_images == false){
        ret = gp_camera_file_delete(this->_camera, path.folder, path. name, this->_ctx);
        gp_file_free(file);
        
        if (ret != GP_OK)
            return ret;
    }
    
    
    int waittime = 10;
    CameraEventType type;
    void *eventdata;
    
    printf("Wait for events from camera\n");
    while(1) {
        
        gp_camera_wait_for_event(this->_camera, waittime, &type, &eventdata, this->_ctx);
        
        if(type == GP_EVENT_TIMEOUT) {
            break;
        }
        else if (type == GP_EVENT_CAPTURE_COMPLETE || type == GP_EVENT_FILE_ADDED) {
            waittime = 10;
        }
        else if (type != GP_EVENT_UNKNOWN) {
            printf("Unexpected event received from camera: %d\n", (int)type);
        }
    }
    
    return true;
}

int CameraController::preview(const char **file_data){
    int ret;
    CameraFile *file;    
    ret = gp_file_new(&file);
    if (ret != GP_OK)
        return ret;
    
	ret = gp_camera_capture_preview(this->_camera, file, this->_ctx);
    
    if(ret != GP_OK)
        return ret;
    
    unsigned long int file_size = 0;
	ret = gp_file_get_data_and_size(file, file_data, &file_size);    
    
    if(ret != GP_OK)
        return ret;

    // FIXME: not working...
    //gp_file_unref(file);
    return (int)file_size;
}

int CameraController::liveview_stop(){
    this->_running_process = false;
    return true;
}

int CameraController::liveview_start(){
    pthread_t tLiveServer;
    if (0 != pthread_create(&tLiveServer, NULL, CameraController::start_liveview_server, this)) {
        return false;
    }    
    return true;
}

int CameraController::bulb(const char *filename, string &data){
    /*
    This doesn't work with a nikon and i've no canon available
     
    int ret;
    CameraEventType evtype;
    std::vector<string> choices;
    
    ret = this->get_settings_choices("shutterspeed2", choices);
    size_t count = choices.size();
    if(ret && count > 0){
        string choice = choices.at(count - 1);
        this->set_settings_value("shutterspeed2", choice.c_str());
    }
    
    ret = this->set_settings_value("d100", "-1");
    
    CameraFile *file;
    CameraFilePath path;
    
    strcpy(path.folder, "/");
	strcpy(path.name, filename);
    
	ret = gp_camera_capture(this->_camera, GP_CAPTURE_IMAGE, &path, this->_ctx);
    if (ret != GP_OK)
        return ret;
    
    time_t lastsec = time(NULL)-3;
    struct timeval	tval;
    while(1){
        ret = this->_wait_and_handle_event(5, &evtype, 1);
    }
    
    if(ret != GP_OK)
        return ret;
    */
    return true;
}

int CameraController::get_files(ptree &tree){
    return this->_get_files(tree, "/");
}

int CameraController::get_file(const char *filename, const char *filepath, string &base64out){
    int ret;
    CameraFile *file;
    
    ret = gp_file_new(&file);
    if(ret < GP_OK)
        return ret;
    
    ret = gp_camera_file_get(this->_camera, filepath, filename, GP_FILE_TYPE_NORMAL, file, this->_ctx);
    if(ret < GP_OK)
        return ret;
    
    this->_file_to_base64(file, base64out);
    return true;
}

int CameraController::_get_files(ptree &tree, const char *path){
    int ret;
    
    const char	*name;
    CameraList *folder, *files;
    
    ret = gp_list_new(&folder);
    if(ret < GP_OK)
        return ret;
    
    ret = gp_camera_folder_list_folders(this->_camera, path,  folder, this->_ctx);
    if(ret < GP_OK)
        return ret;
    
    int count_folders = gp_list_count(folder);
    
    if(count_folders){
        for(int i = 0; i < count_folders; i++){
            gp_list_get_name(folder, i, &name);
            
            string abspath(path);
            abspath.append(name);
            abspath.append("/");
            
            this->_get_files(tree, abspath.c_str());
        }
    } else {
        string show_thumbnials = Settings::get_value("general.thumbnail");
        
        ret = gp_list_new(&files);
        if(ret < GP_OK)
            return ret;
        
        ret = gp_camera_folder_list_files(this->_camera, path, files, this->_ctx);
        if(ret < GP_OK)
            return ret;
        
        int count_files = gp_list_count(files);
        
        ptree current_folder, filelist;
        current_folder.put("absolute_path", path);
        
        string thumb_widht = Settings::get_value("general.thumbnail_width");
        string thumb_height = Settings::get_value("general.thumbnail_width");
        bool show_thumb = (show_thumbnials.compare("true") == 0);
        
        for(int j = 0; j < count_files; j++){
            gp_list_get_name(files, j, &name);
            
            ptree valuechild;
            valuechild.put("name", name);
        
            const char *ext = strrchr(name, '.');
            if(show_thumb && (strcmp(".jpg", ext) == 0 || strcmp(".jpeg", ext) == 0 || strcmp(".JPG", ext) == 0  )){
                CameraFilePath cPath;
                strcpy(cPath.folder, path);
                strcpy(cPath.name, name);
                
                printf("start read file %s", name);
                
                CameraFile *file;
                ret = gp_file_new(&file);
                
                if (ret != GP_OK)
                    continue;
            
                ret = gp_camera_file_get(this->_camera, cPath.folder, cPath.name, GP_FILE_TYPE_NORMAL, file, this->_ctx);
            
                if (ret != GP_OK)
                    continue;
                
                unsigned long int file_size = 0;
                const char *file_data = NULL;
                
                ret = gp_file_get_data_and_size (file, &file_data, &file_size);
                if (ret != GP_OK)
                    continue;

                string thumb = "";
                Helper::resize_image_to_base64(atoi(thumb_widht.c_str()), atoi(thumb_widht.c_str()), file_data, file_size, thumb);
                valuechild.put("thumbnail", thumb);
                
                printf("end read file %s", name);
                
                gp_file_free(file);
            }
            
            filelist.push_back(std::make_pair("", valuechild));
        }
        current_folder.put_child("files", filelist);        
        tree.put_child("folder", current_folder);
        gp_list_free(files);

    }
    gp_list_free(folder);
    
    return true;
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

int CameraController::get_settings_choices(const char *key, vector<string> &choices){
    CameraWidget *w, *child;
    int ret;
    
    ret = gp_camera_get_config(this->_camera, &w, this->_ctx);
    if(ret < GP_OK){
        return ret;
    }
    
    ret = gp_widget_get_child_by_name(w, key, &child);
    if(ret < GP_OK)
        return ret;
    
    int count = gp_widget_count_choices(child);
    for(int i = 0; i < count; i++){
        const char *choice;
        ret = gp_widget_get_choice(child, i, &choice);
        
        if(ret < GP_OK)
            return ret;
        
        choices.push_back(string(choice));
    }    
    return true;
}

int CameraController::get_settings_value(const char *key, string &val){
    CameraWidget *w, *child;
    int ret;
    
    ret = gp_camera_get_config(this->_camera, &w, this->_ctx);
    if(ret < GP_OK){
        return ret;
    }
    
    ret = gp_widget_get_child_by_name(w, key, &child);
    if(ret < GP_OK)
        return ret;
    
    void *item_value;
    ret = gp_widget_get_value(child, &item_value);
    if(ret < GP_OK)
        return ret;
    
    unsigned char *value = static_cast<unsigned char *>(item_value);
    val.append((char *)value);
    
    return true;
}

int CameraController::set_settings_value(const char *key, const char *val){

    CameraWidget *w, *child;
    int ret = gp_camera_get_config(this->_camera, &w, this->_ctx);
    if(ret < GP_OK)
        return false;
    
    ret = gp_widget_get_child_by_name(w, key, &child);
    if(ret < GP_OK)
        return false;
    
    ret = gp_widget_set_value(child, val);
    if(ret < GP_OK)
        return false;
    
    
    ret = gp_camera_set_config(this->_camera, w, this->_ctx);
    
    gp_widget_free(w);
    return (ret == GP_OK);
}


/**
 borrowed gphoto2
 http://sourceforge.net/p/gphoto/code/HEAD/tree/trunk/gphoto2/gphoto2/main.c#l834
 */
int CameraController::_wait_and_handle_event (long waittime, CameraEventType *type, int download) {
	int 		result;
	CameraEventType	evtype;
	void		*data;
	CameraFilePath	*path;
    
	if (!type) type = &evtype;
	evtype = GP_EVENT_UNKNOWN;
	data = NULL;
	result = gp_camera_wait_for_event(this->_camera, waittime, type, &data, this->_ctx);
	if (result == GP_ERROR_NOT_SUPPORTED) {
		*type = GP_EVENT_TIMEOUT;
		usleep(waittime*1000);
		return GP_OK;
	}
	if (result != GP_OK)
		return result;
	path = (CameraFilePath*)data;
	switch (*type) {
        case GP_EVENT_TIMEOUT:
            break;
        case GP_EVENT_CAPTURE_COMPLETE:
            break;
        case GP_EVENT_FOLDER_ADDED:
            free (data);
            break;
        case GP_EVENT_FILE_ADDED:
            //result = save_captured_file (path, download);
            free (data);
            /* result will fall through to final return */
            break;
        case GP_EVENT_UNKNOWN:
#if 0 /* too much spam for the common usage */
            printf (_("Event UNKNOWN %s during wait, ignoring.\n"), (char*)data);
#endif
            free (data);
            break;
        default:           
            break;
	}
	return result;
}

void CameraController::_set_capturetarget(int index){
    vector<string> choices;
    
    int ret;
    ret = this->get_settings_choices("capturetarget", choices);
    
    if(ret && index < choices.size()){
        string choice = choices.at(index);
        this->set_settings_value("capturetarget", choice.c_str());
    }
    
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
            int val = *((int*)&item_value);
            tree.put<int>("value", val);
            break;
        }
            
        case GP_WIDGET_RANGE: {
            float val = *((float*)&item_value);
            tree.put<float>("value", val);
            break;
        }
            
        default:
            break;
    }
}

int CameraController::_file_to_base64(CameraFile *file, string &output){
    int ret;
    unsigned long int file_size = 0;
    const char *file_data = NULL;
    
	ret = gp_file_get_data_and_size (file, &file_data, &file_size);
    if (ret != GP_OK)
        return ret;
    //char *dest = new char[file_size];
    char *dest = (char*)malloc(file_size * sizeof(char*));
    base64_encode(dest, (char*)file_data, (int)file_size);
    output.append(dest);
    free(dest);
    
    return true;
}

int CameraController::_file_to_base64(const char* data, unsigned int data_size, string &output){
    //char *dest = new char[file_size];
    char *dest = (char*)malloc(data_size * sizeof(char*));
    base64_encode(dest, (char*)data, (int)data_size);
    output.append(dest);
    free(dest);
    
    return true;
}

void* CameraController::start_liveview_server(void *context){
    CameraController *cc = (CameraController *)context;
    cc->_running_process = true;
    
    string port;
    string host;
    Settings *sett = Settings::getInstance();
    sett->get_value("preview.host", host);
    sett->get_value("preview.remote_port", port);
    
    io_service io_s;
    ip::address_v4 target;
    
    target.from_string(host);
    
    ip::tcp::endpoint endpoint(ip::tcp::v4(), atoi(port.c_str()));
    endpoint.address(target);
    
    ip::tcp::acceptor acceptor(io_s, endpoint);
    ip::tcp::socket sock(io_s);
    
    try{
        acceptor.accept(sock);
        while(cc->_running_process){
            const char *data = NULL;
            int size = cc->preview(&data);

            if(size == 0) continue;
            else if(size < 0)break;
            
            printf("--------------------------------------\n");
            printf("\n%d\n", size);
            printf("--------------------------------------\n");            
        
            write(sock, buffer(&size, 4));
            write(sock, buffer(data, size));
        }
    } catch(std::exception& e){
        printf("error %s:",e.what());
        cc->_running_process = false;
    }
    
    sock.close();    
    gp_camera_exit(cc->_camera, cc->_ctx);
    
    return NULL;
}

GPContextErrorFunc CameraController::_error_callback(GPContext *context, const char *text, void *data){
    
    return 0;
}

GPContextMessageFunc CameraController::_message_callback(GPContext *context, const char *text, void *data){
    
    return 0;
}
