//
//  Helper.cpp
//  CameraControllerApi
//
//  Created by Tobias Scheck on 16.10.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#include "Helper.h"
#include "Base64.h"
#include <stdlib.h>
#include <exiv2/exiv2.hpp>
#include <assert.h>

using namespace CameraControllerApi;

bool Helper::get_image_from_exif(const char *image_data, unsigned long data_size, string &base) {
    return Helper::get_thumbnail_from_exif(image_data, data_size, base, FULL);
}

bool Helper::get_thumbnail_from_exif(const char *image_data, unsigned long data_size, string &base, ImageType type) {
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const Exiv2::byte*)image_data, data_size);
        assert(image.get() != 0);
        image->readMetadata();
        
        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) {
            std::cerr << "exif data are empty" << std::endl;
            return false;
        }
        
        Exiv2::PreviewManager loader(*image);
        Exiv2::PreviewPropertiesList list = loader.getPreviewProperties();
        
        Exiv2::PreviewProperties thumb;
        unsigned long start = type == THUMBNAIL ? 0 : list.size() - 1;
        unsigned long end = type == THUMBNAIL ? list.size() - 1 : 0;
        
        for(unsigned long i = start; i != end; type == THUMBNAIL ? ++i : --i)
        {
            thumb = list.at(i);
            if(thumb.extension_.compare(".jpg") == 0) {
                std::cerr << "Preview Item: " << thumb.width_ << "x" << thumb.height_ << " ext:" << thumb.extension_ << std::endl;
                break;
            }
        }
        
        Exiv2::PreviewImage preview = loader.getPreviewImage(thumb);
        if(preview.size() <= 0){
            std::cerr << "exif thumbnail is empty" << std::endl;
            return false;
        }
        
        Exiv2::DataBuf buf = preview.copy();
        std::string *data = new std::string(reinterpret_cast<char*>(buf.pData_), buf.size_);
        Helper::to_base64(data, base);
        
        delete data;
        buf.release();
        
        return true;
    } catch (Exiv2::Error& e) {
        std::cerr << "error in image: " << e.what() << " " << std::endl;
        return false;
    }
}

void Helper::to_base64(std::string *data, string &base) {
    char *buffer = new char[data->size() * sizeof(char*)];
    if(buffer == NULL) return;
    
    base64_encode(buffer, data->c_str(), static_cast<int>(data->size()));
    base.append(buffer);
    
    delete[] buffer;
}