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

/**
 * @brief Creates a new blank Netpbm image.
 *
 * Allocates memory for a NetpbmImage structure and initializes the pixel data.
 *
 * @param type The type of the image (PBM, PGM, or PPM).
 * @param format The format of the image (PLAIN or RAW).
 * @param width The width of the image in pixels.
 * @param height The height of the image in pixels.
 * @param max_value The maximum pixel value (should be 1 for PBM).
 * @return Pointer to the created NetpbmImage, or NULL if allocation fails.
 *
 * @note The returned image must be freed using netpbm_destroy().
 */
NetpbmImage *netpbm_create(NetpbmType type, NetpbmFormat format, int width, int height, int max_value);

/**
 * @brief Frees the memory allocated for a NetpbmImage.
 *
 * Deallocates the memory used by the image data and the NetpbmImage structure itself.
 *
 * @param img Pointer to the pointer of the NetpbmImage to be freed. Will be set to NULL after freeing.
 */
void netpbm_free(NetpbmImage **img);

#endif