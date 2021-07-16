#pragma once

#include "scene/components.hpp"
#include "serialize/image.capnp.h"
#include "serialize/mesh.capnp.h"
#include "serialize/vec_readers.hpp"

#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

static inline void process_image(const char *in, const char *out)
{
    int x, y, n;

    uint8_t *data = stbi_load(in, &x, &y, &n, 0);

    if (data == nullptr) {
        std::cerr << "Error loading image: " << stbi_failure_reason() << "\n";
        exit(-1);
    }

    std::string out_file(out);

    out_file.append(".fwtex");

    FILE *fp = fopen(out_file.c_str(), "w");

    ::capnp::MallocMessageBuilder msg;

    fw::capnp::Image::Builder img_ser = msg.initRoot<fw::capnp::Image>();

    auto img_data = img_ser.initCompressedData(x * y * n);

    memcpy(img_data.begin(), data, x * y * n);

    stbi_image_free(data);

    img_ser.initSize();
    auto size = img_ser.getSize();
    size.setX(x);
    size.setY(y);

    fw::capnp::Image::ColorFormat fmt;

    switch (n) {
    case 1:
        fmt = fw::capnp::Image::ColorFormat::FORMAT_R;
        break;
    case 2:
        fmt = fw::capnp::Image::ColorFormat::FORMAT_RG;
        break;
    case 3:
        fmt = fw::capnp::Image::ColorFormat::FORMAT_RGB;
        break;
    case 4:
        fmt = fw::capnp::Image::ColorFormat::FORMAT_RGBA;
        break;
    }

    img_ser.setCompression(fw::capnp::Image::Compression::COMP_NONE);
    img_ser.setColorFormat(fmt);

    ::capnp::writePackedMessageToFd(fileno(fp), msg);

    fclose(fp);
}