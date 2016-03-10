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
//#include <FreeImage.h>
#include <libexif/exif-loader.h>

using namespace CameraControllerApi;


void Helper::get_thumbnail_from_exif(const char *image_data, unsigned long data_size, string &base){
    ExifLoader *l = exif_loader_new();
    if(l){
        ExifData *exif_data;
        exif_loader_write(l, (unsigned char*)image_data, (unsigned int)data_size);
        exif_data = exif_loader_get_data(l);
        exif_loader_unref(l);
        l = NULL;
        
        if(exif_data){
            if(exif_data->data && exif_data->size){
                char *buffer = new char[exif_data->size * sizeof(unsigned char*)];
                if(buffer == NULL) return;
                
                base64_encode(buffer, (char *)exif_data->data, (int)exif_data->size);
                base.append(buffer);
                delete[] buffer;
            }
            exif_data_free(exif_data);
        }
    }    
}

/*void Helper::resize_image_to_base64(int w, int h, const char *image_data, unsigned long data_size, string &base){
    Helper::_resize(w, h, image_data, data_size, base);
}

void Helper::resize_image_to_base64(int square, const char *image_data, unsigned long data_size, string &base){
    Helper::_resize(square, square, image_data, data_size, base);
}

void Helper::_resize(int w, int h, const char *image_data, unsigned long data_size, string &base){
    FreeImage_Initialise();
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
    FreeImage_DeInitialise();
}
 
// raw libjpeg implementation... not working
void Helper::_resize(int w, int h, const char *image_data, unsigned long data_size, string &base){
    struct jpeg_decompress_struct in;
    struct jpeg_error_mgr in_err;
    
    in.err = jpeg_std_error(&in_err);
    jpeg_create_decompress(&in);
    
    jpeg_mem_src(&in, (unsigned char*)image_data, data_size);
    
    if(jpeg_read_header(&in, TRUE) == false)
        return;
    
    jpeg_start_decompress(&in);
    
    JSAMPROW row_pointer[1];
    JSAMPARRAY buffer;
    int width, height, pixel_size, row_stride, fx, fy;
    
    pixel_size = in.num_components;
    width = in.output_width;
    height = in.output_height;
    
    fx = width / w;
    fy = height / h;
    
    row_stride = w * pixel_size;
    buffer = (*in.mem->alloc_sarray)((j_common_ptr) &in,JPOOL_IMAGE, row_stride, 1);
    
    while (in.output_scanline < height) {
        jpeg_read_scanlines(&in, buffer, 1);
        for(int x = 0; x < w; x++){
            
        }
    }
    
}*/