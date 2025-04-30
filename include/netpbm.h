#ifndef NETPBM_H
#define NETPBM_H

/** convenience type defintion of byte */
typedef unsigned char byte;

/**
 * @brief Enumeration of Netpbm image types.
 * 
 * The type of the image determines the color depth and format.
 * 
 */
typedef enum {
    /// Bitmap (black and white)
    PBM,
    /// Grayscale
    PGM,
    /// Color
    PPM
} NetpbmType;

/**
 * @brief Enumeration of Netpbm image formats.
 * 
 * The image format determines how the pixel data is stored.
 * 
 */
typedef enum {
    /// ASCII pixel format */
    PLAIN,
    /// Binary pixel format */
    RAW
} NetpbmFormat;

/**
 * @brief Structure representing a color in RGB format.
 * 
 * Each color component is represented as a byte (0-255).
 * 
 */
typedef struct {
    /// Red component
    byte r;
    /// Green component
    byte g;
    /// Blue component
    byte b;
} Color;

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
        byte *bitmap_data;
        /// PGM: 1 byte per pixel (or 2 bytes if max_value > 255)
        byte *gray_data;
        /// PPM: 3 bytes per pixel
        Color *color_data;
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
 * @return 0 on success, or -1 on failure.
 */
int netpbm_save(const NetpbmImage *img, const char *filename);

/**
 * @brief Loads a Netpbm image from a file.
 *
 * Reads the image data from a file and populates a NetpbmImage structure.
 *
 * @param filename The name of the file to load the image from.
 * @param img Pointer to the pointer of the NetpbmImage to be populated. Will be allocated if NULL.
 * @return 0 on success, or -1 on failure.
 */
int netpbm_load(const char *filename, NetpbmImage **img);

#endif