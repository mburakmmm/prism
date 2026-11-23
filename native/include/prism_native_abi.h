#ifndef PRISM_NATIVE_ABI_H
#define PRISM_NATIVE_ABI_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PRISM_NATIVE_ABI_VERSION 1u

typedef enum PrismNativeStatus {
    PRISM_NATIVE_OK = 0,
    PRISM_NATIVE_ERR_INVALID_ARG = 1,
    PRISM_NATIVE_ERR_CORRUPT_DATA = 2,
    PRISM_NATIVE_ERR_RESOURCE_LIMIT = 3,
    PRISM_NATIVE_ERR_UNSUPPORTED_FORMAT = 4,
    PRISM_NATIVE_ERR_OUT_OF_MEMORY = 5,
    PRISM_NATIVE_ERR_INTERNAL = 6
} PrismNativeStatus;

typedef struct PrismNativeLimits {
    uint32_t max_width;
    uint32_t max_height;
    uint64_t max_pixels;
    uint64_t max_decoded_bytes;
    uint64_t max_output_bytes;
    uint64_t max_metadata_bytes;
    uint32_t max_work_ms;
} PrismNativeLimits;

typedef struct PrismNativeError {
    PrismNativeStatus status;
    char message[256];
} PrismNativeError;

typedef struct PrismNativeBuffer {
    uint8_t *data;
    size_t len;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint8_t channels;
    uint8_t orientation;
    void (*free_fn)(uint8_t *);
} PrismNativeBuffer;

uint32_t prism_native_abi_version(void);

void prism_native_free_buffer(PrismNativeBuffer *buf);

PrismNativeStatus prism_native_jpeg_decode(
    const uint8_t *input,
    size_t input_len,
    const PrismNativeLimits *limits,
    uint8_t strip_private_metadata,
    PrismNativeBuffer *out_buf,
    PrismNativeError *out_err
);

PrismNativeStatus prism_native_jpeg_encode(
    const uint8_t *pixels,
    size_t pixels_len,
    uint32_t width,
    uint32_t height,
    uint32_t stride,
    uint8_t channels,
    uint8_t quality,
    uint8_t subsampling,
    const PrismNativeLimits *limits,
    PrismNativeBuffer *out_buf,
    PrismNativeError *out_err
);

PrismNativeStatus prism_native_webp_decode(
    const uint8_t *input,
    size_t input_len,
    const PrismNativeLimits *limits,
    PrismNativeBuffer *out_buf,
    PrismNativeError *out_err
);

PrismNativeStatus prism_native_webp_encode(
    const uint8_t *pixels,
    size_t pixels_len,
    uint32_t width,
    uint32_t height,
    uint32_t stride,
    uint8_t channels,
    uint8_t quality,
    uint8_t lossless,
    const PrismNativeLimits *limits,
    PrismNativeBuffer *out_buf,
    PrismNativeError *out_err
);

PrismNativeStatus prism_native_tiff_decode(
    const uint8_t *input,
    size_t input_len,
    const PrismNativeLimits *limits,
    PrismNativeBuffer *out_buf,
    PrismNativeError *out_err
);

PrismNativeStatus prism_native_tiff_encode(
    const uint8_t *pixels,
    size_t pixels_len,
    uint32_t width,
    uint32_t height,
    uint32_t stride,
    uint8_t channels,
    uint8_t compression,
    const PrismNativeLimits *limits,
    PrismNativeBuffer *out_buf,
    PrismNativeError *out_err
);

#ifdef __cplusplus
}
#endif

#endif /* PRISM_NATIVE_ABI_H */
