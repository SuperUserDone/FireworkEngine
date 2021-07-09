#include "scene/components.hpp"
#include "serialize/image.capnp.c++"
#include "serialize/image.capnp.h"
#include "serialize/mesh.capnp.c++"
#include "serialize/mesh.capnp.h"
#include "serialize/vec_readers.hpp"
#include "serialize/vector.capnp.c++"

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

void print_help(const char *str)
{
    std::cout << "Usage " << str << "<Mode> <Input> <Output>\n"
              << "Mode: Mode of \n"
              << "\t-> Image\n"
              << "\t-> Mesh\n"
              << "Input: input file\n"
              << "Output: output file\n";
}

enum Mode
{
    MODE_ERR,
    MODE_IMAGE,
    MODE_MESH
};

Mode to_mode(const char *mode)
{
    std::string mode_cppstr(mode);
    std::for_each(mode_cppstr.begin(), mode_cppstr.end(), [](char &c) { c = tolower(c); });
    if (mode_cppstr == std::string("image")) return MODE_IMAGE;
    if (mode_cppstr == std::string("mesh")) return MODE_MESH;

    return MODE_ERR;
}

void process_mesh(const char *in, const char *out)
{
    Assimp::Importer importer;

    unsigned int flags = 0;
    flags |= aiProcess_CalcTangentSpace;
    flags |= aiProcess_Triangulate;
    flags |= aiProcess_GenNormals;
    flags |= aiProcess_CalcTangentSpace;
    flags |= aiProcess_JoinIdenticalVertices;
    flags |= aiProcess_SortByPType;
    flags |= aiProcess_FlipUVs;

    const aiScene *scene = importer.ReadFile(in, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Error loading mesh: " << importer.GetErrorString() << "\n";
        return;
    }

    if (scene->mNumMeshes != 1) {
        // TODO FIX
        std::cout << "Currently, models with mesh count != 1 not supported\n";
        return;
    }

    const aiMesh *mesh = scene->mMeshes[0];

    {
        std::string out_file(out);
        out_file.append(".fwmesh");
        FILE *fp = fopen(out_file.c_str(), "w");

        ::capnp::MallocMessageBuilder msg;
        fw::capnp::Mesh::Builder mesh_ser = msg.initRoot<fw::capnp::Mesh>();

        auto verticies = mesh_ser.initVerticies(mesh->mNumVertices);
        auto indicies = mesh_ser.initIndicies(mesh->mNumFaces * 3);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            fw::vertex vert_data;

            fw::convert_vec3(mesh->mVertices[i], vert_data.pos);
            fw::convert_vec3(mesh->mNormals[i], vert_data.norm);
            fw::convert_vec3(mesh->mTangents[i], vert_data.tan);

            if (mesh->mTextureCoords[0])
                fw::convert_vec2(mesh->mVertices[i], vert_data.uvs);
            else
                vert_data.uvs = glm::vec2(0.f, 0.f);

            auto vert = verticies[i];
            fw::write_vertex(vert_data, vert);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indicies.set((i * 3) + j, face.mIndices[j]);
        }

        ::capnp::writePackedMessageToFd(fileno(fp), msg);
        fclose(fp);
    }
}

void process_image(const char *in, const char *out)
{
    std::cout << "Processing image: " << in << " to " << out << "\n";

    int x, y, n;

    uint8_t *data = stbi_load(in, &x, &y, &n, 0);

    if (data == nullptr) {
        std::cout << "Error loading image: " << stbi_failure_reason() << "\n";
        return;
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

    std::cout << "Done!\n";
}

int main(int argc, char *argv[])
{
    if (argc < 4) print_help(argv[0]);

    switch (to_mode(argv[1])) {
    case MODE_ERR:
        print_help(argv[0]);
        break;
    case MODE_MESH:
        process_mesh(argv[2], argv[3]);
        break;
    case MODE_IMAGE:
        process_image(argv[2], argv[3]);
        break;
    }

    return 0;
}
