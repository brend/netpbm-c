#include "netpbm.h"
#include <stdlib.h>
#include <string.h>

// Create a new image with the specified type, format, width, height, and max value
// Returns NULL if memory allocation fails
// The caller is responsible for freeing the allocated memory
// The image data is initialized to zero
NetpbmImage *netpbm_create(NetpbmType type, NetpbmFormat format, int width, int height, int max_value) {
    NetpbmImage *img = (NetpbmImage *)malloc(sizeof(NetpbmImage));

    // Check if memory allocation was successful
    if (img == NULL) {
        return NULL;
    }

    // Initialize the image properties
    img->type = type;
    img->format = format;
    img->width = width;
    img->height = height;
    img->max_value = max_value;

    // Allocate memory for the image data
    switch (type) {
        case PBM:
            img->data.bitmap_data = (byte *)malloc(width * height / 8);
            break;
        case PGM:
            img->data.gray_data = (byte *)malloc(width * height * (max_value > 255 ? 2 : 1));
            break;
        case PPM:
            img->data.color_data = (Color *)malloc(width * height * sizeof(Color));
            break;
        default:
            free(img);
            return NULL;
    }

    // Check if memory allocation was successful
    if (img->data.bitmap_data == NULL && img->data.gray_data == NULL && img->data.color_data == NULL) {
        free(img);
        return NULL;
    }

    // Initialize the image data
    switch (type) {
        case PBM:
            memset(img->data.bitmap_data, 0, width * height / 8);
            break;
        case PGM:
            memset(img->data.gray_data, 0, width * height * (max_value > 255 ? 2 : 1));
            break;
        case PPM:
            for (int i = 0; i < width * height; i++) {
                img->data.color_data[i].r = 0;
                img->data.color_data[i].g = 0;
                img->data.color_data[i].b = 0;
            }
            break;
    }

    return img;
}

// Free the allocated memory for the image
void netpbm_free(NetpbmImage **img) {
    if (*img != NULL) {
        switch ((*img)->type) {
            case PBM:
                free((*img)->data.bitmap_data);
                break;
            case PGM:
                free((*img)->data.gray_data);
                break;
            case PPM:
                free((*img)->data.color_data);
                break;
        }
        free(*img);
    }
    *img = NULL;
}
