//
//  Helper.h
//  CameraControllerApi
//
//  Created by Tobias Scheck on 16.10.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#ifndef __CameraControllerApi__Helper__
#define __CameraControllerApi__Helper__

#include <iostream>
#include <string>

using std::string;

namespace CameraControllerApi {
    class Helper {
    public:
        typedef enum {
            THUMBNAIL,
            FULL
        } ImageType;
        //static void resize_image_to_base64(int w, int h, const char *image_data, unsigned long data_size, string &base);
        //static void resize_image_to_base64(int square, const char *image_data, unsigned long data_size, string &base);
        static bool get_thumbnail_from_exif(const char *image_data, unsigned long data_size, string &base, ImageType type = THUMBNAIL);
        static bool get_image_from_exif(const char *image_data, unsigned long data_size, string &base);
        static void to_base64(std::string *data, string &base);
        
    private:
        //static void _resize(int w, int h, const char *image_data, unsigned long data_size, string &base);
    };
}

#endif /* defined(__CameraControllerApi__Helper__) */
