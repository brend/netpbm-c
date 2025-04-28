#ifndef NETPBM_H
#define NETPBM_H

typedef unsigned char byte;

typedef enum {
    // Bitmap (black and white)
    PBM,
    // Grayscale
    PGM,
    // Color
    PPM
} NetpbmType;

typedef enum {
    // ASCII
    PLAIN,
    // Binary
    RAW
} NetpbmFormat;

typedef struct {
    byte r;
    byte g;
    byte b;
} Color;

typedef struct Image {
    NetpbmType type;
    NetpbmFormat format;
    int width;
    int height;
    // Only used for PGM and PPM (PGM/PPM can have max value != 255)
    int max_value;

    union {
        // PBM: 1 byte per pixel (0 or 1)
        byte *bitmap_data;
        // PGM: 1 byte per pixel (or 2 bytes if max_value > 255)
        byte *gray_data;
        // PPM: 3 bytes per pixel
        Color *color_data;
    } data;
} NetpbmImage;

// Create a new image with the specified type, format, width, height, and max value
// Returns NULL if memory allocation fails
// The caller is responsible for freeing the allocated memory
// The image data is initialized to zero
NetpbmImage *netpbm_create(NetpbmType type, NetpbmFormat format, int width, int height, int max_value);

// Free the allocated memory for the image
void netpbm_free(NetpbmImage **img);

#endif