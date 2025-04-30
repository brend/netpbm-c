#ifndef NETPBM_H
#define NETPBM_H

/**
 * @file netpbm.h
 * @brief Header file for Netpbm image handling.
 *
 * This file contains the definitions and function prototypes for creating,
 * manipulating, and saving Netpbm images.
 *
 * @note This library supports PBM, PGM, and PPM formats.
 */

 /**
  * @brief Enumeration of error codes for Netpbm operations.
  */
 typedef enum {
    NETPBM_SUCCESS = 0,
    NETPBM_ERROR_MEMORY_ALLOCATION = -1,
    NETPBM_ERROR_FILE_NOT_FOUND = -2,
    NETPBM_ERROR_UNSUPPORTED_FORMAT = -3,
    NETPBM_ERROR_INVALID_DATA = -4,
    NETPBM_ERROR_IO = -5,
    NETPBM_ERROR_UNSUPPORTED_TYPE = -6,
 } NetpbmError;

/**
 * @brief Enumeration of Netpbm image types.
 * 
 * The type of the image determines the color depth and format.
 * 
 */
typedef enum {
    /// Bitmap (black and white)
    NETPBM_TYPE_PBM,
    /// Grayscale
    NETPBM_TYPE_PGM,
    /// Color
    NETPBM_TYPE_PPM
} NetpbmType;

/**
 * @brief Enumeration of Netpbm image formats.
 * 
 * The image format determines how the pixel data is stored.
 * 
 */
typedef enum {
    /// ASCII pixel format */
    NETPBM_FORMAT_PLAIN,
    /// Binary pixel format */
    NETPBM_FORMAT_RAW
} NetpbmFormat;

/**
 * @brief Structure representing a color in RGB format.
 * 
 * Each color component is represented as a byte (0-255).
 * 
 */
typedef struct {
    /// Red component
    unsigned char r;
    /// Green component
    unsigned char g;
    /// Blue component
    unsigned char b;
} NetpbmColor;

/**
 * @brief Structure representing a Netpbm image.
 * 
 * This structure contains the image type, format, dimensions, and pixel data.
 * 
 */
typedef struct Image {
    /// Type of the image (PBM, PGM, or PPM)
    NetpbmType type;
    /// Format of the image (PLAIN or RAW)
    NetpbmFormat format;
    /// Width of the image in pixels
    int width;
    /// Height of the image in pixels
    int height;
    /** Maximum pixel value (for PGM and PPM) 
     * Only used for PGM and PPM (PGM/PPM can have max value != 255)
    */
    int max_value;

    /** Pointer to the pixel data */
    /** 
     * The data is stored in a union to allow for different types of pixel data.
     * Depending on the image type, the appropriate member of the union will be used.
     */
    union {
        /// PBM: 1 byte per pixel (0 or 1)
        unsigned char *bitmap_data;
        /// PGM: 1 byte per pixel (or 2 bytes if max_value > 255)
        unsigned char *gray_data;
        /// PPM: 3 bytes per pixel
        NetpbmColor *color_data;
    } data;
} NetpbmImage;

/**
 * @brief Creates a new Netpbm image.
 *
 * Allocates memory for a NetpbmImage structure.
 *
 * @param type The type of the image (PBM, PGM, or PPM).
 * @param format The format of the image (PLAIN or RAW).
 * @param width The width of the image in pixels.
 * @param height The height of the image in pixels.
 * @param max_value The maximum pixel value (should be 1 for PBM).
 * @param zero_fill If non-zero, the image data will be initialized to zero.
 * @return Pointer to the created NetpbmImage, or NULL if allocation fails.
 *
 * @note The returned image must be freed using netpbm_destroy().
 */
NetpbmImage *netpbm_create_ex(
    NetpbmType type, 
    NetpbmFormat format, 
    int width, 
    int height, 
    int max_value,
    int zero_fill
);

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

/**
 * @brief Saves a Netpbm image to a file.
 *
 * Writes the image data to a file in the specified format.
 *
 * @param img Pointer to the NetpbmImage to be saved.
 * @param filename The name of the file to save the image to.
 * @return Success or failure code.
 */
NetpbmError netpbm_save(const NetpbmImage *img, const char *filename);

/**
 * @brief Loads a Netpbm image from a file.
 *
 * Reads the image data from a file and populates a NetpbmImage structure.
 *
 * @param filename The name of the file to load the image from.
 * @param img Pointer to the pointer of the NetpbmImage to be populated. Will be allocated if NULL.
 * @return Success or failure code.
 */
NetpbmError netpbm_load(const char *filename, NetpbmImage **img);

#endif