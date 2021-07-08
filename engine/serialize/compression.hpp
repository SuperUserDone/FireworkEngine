#pragma once

#include <stdint.h>

enum compression_format : uint8_t
{
    COMPRESSED_NONE,
    COMPRESSED_LZ4
};

uint8_t *decomp_data(const uint8_t *data, compression_format fmt) { return nullptr; }
uint8_t *comp_data(const uint8_t *data, compression_format fmt) { return nullptr; }
