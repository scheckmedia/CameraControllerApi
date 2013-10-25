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
#include <FreeImage.h>

using namespace CameraControllerApi;

void Helper::resize_image_to_base64(int w, int h, const char *image_data, unsigned long data_size, string &base){
    Helper::_resize(w, h, image_data, data_size, base);
}

void Helper::resize_image_to_base64(int square, const char *image_data, unsigned long data_size, string &base){
    Helper::_resize(square, square, image_data, data_size, base);
}

void Helper::_resize(int w, int h, const char *image_data, unsigned long data_size, string &base){
    FIMEMORY *src = FreeImage_OpenMemory((BYTE *)image_data, (DWORD)data_size);
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(src);
    FIBITMAP *image = FreeImage_LoadFromMemory(fif, src);
    if(image){
        FIMEMORY *dest = FreeImage_OpenMemory();
        FIBITMAP *thumbnail = FreeImage_Rescale(image, w, h, FILTER_BOX);
        if(thumbnail){
            FreeImage_SaveToMemory(fif, thumbnail, dest, JPEG_DEFAULT);
            
            BYTE *mem_buffer = NULL;
            DWORD size_in_bytes = 0;
            
            FreeImage_AcquireMemory(dest, &mem_buffer, &size_in_bytes);
            
            char *buffer = new char[size_in_bytes * sizeof(BYTE*)];
            if(buffer == NULL) return;
            
            base64_encode(buffer, (char *)mem_buffer, (int)size_in_bytes);
            base.append(buffer);
            delete[] buffer;
            
        }
        FreeImage_Unload(thumbnail);
        FreeImage_CloseMemory(dest);
    }
    FreeImage_Unload(image);
    FreeImage_CloseMemory(src);
}