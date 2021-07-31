#pragma once

#include "process_image.hpp"
#include "scene/components.hpp"
#include "serialize/mesh.capnp.h"
#include "serialize/vec_readers.hpp"

#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <filesystem>
#include <iostream>
#include <json.hpp>
#include <string>

using namespace nlohmann;

static inline std::unordered_map<std::string, std::string> textures;

static inline void process_texture(const aiMaterial *mat,
                                   const aiTextureType type,
                                   const std::string name,
                                   json &j)
{
    aiString path;
    mat->GetTexture(type, 0, &path);

    std::string filepath = path.C_Str();
    std::string tex_name =
        std::filesystem::path(path.C_Str()).filename().replace_extension().string() + "_" + name;

    j[name] = tex_name;

    textures[tex_name] = filepath;
}

static inline json process_material(const aiMaterial *mat, const aiScene *scene)
{
    json j;

    aiString name;

    mat->Get(AI_MATKEY_NAME, name);

    j["name"] = name.C_Str();

    if (mat->GetTextureCount(aiTextureType_DIFFUSE) == 1) {
        process_texture(mat, aiTextureType_DIFFUSE, "diffuse", j);
    }

    return j;
}

static inline json process_node(const aiNode *node, const aiScene *scene)
{
    json j;

    j["name"] = node->mName.C_Str();

    if (node->mNumMeshes > 1) {
        // FIXME
        std::cerr << "[FIXME] Error, node " << j["name"] << " can only have 1 or 0 meshes!\n";
        exit(-1);
    } else if (node->mNumMeshes != 1) {
        for (int i = 0; i < node->mNumChildren; i++) {
            j["children"][i] = process_node(node->mChildren[i], scene);
        }

        return j;
    }

    j["mesh"] = scene->mMeshes[node->mMeshes[0]]->mName.C_Str();

    aiVector3t<float> scale;
    aiVector3t<float> pos;
    aiQuaterniont<float> rot;

    node->mTransformation.Decompose(scale, rot, pos);

    j["transform"]["pos"]["x"] = pos.x;
    j["transform"]["pos"]["y"] = pos.y;
    j["transform"]["pos"]["z"] = pos.z;

    j["transform"]["scale"]["x"] = scale.x;
    j["transform"]["scale"]["y"] = scale.y;
    j["transform"]["scale"]["z"] = scale.z;

    j["transform"]["rot"]["x"] = rot.x;
    j["transform"]["rot"]["y"] = rot.y;
    j["transform"]["rot"]["z"] = rot.z;
    j["transform"]["rot"]["w"] = rot.w;

    return j;
}

static inline std::pair<std::string, std::string> process_mesh(const aiMesh *mesh,
                                                               const std::string &path)
{
    std::filesystem::path out_file(path);
    std::filesystem::create_directories(out_file);

    out_file.append(std::string(mesh->mName.C_Str()) + ".fwmesh");

    FILE *fp = fopen(out_file.c_str(), "wb");

    if (!fp) {
        std::cerr << "Error opening mesh for writing: " << out_file.string() << std::endl;
        exit(-1);
    }

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

    return std::make_pair(out_file.string(), mesh->mName.C_Str());
}

static inline void process_scene(const char *in, const char *out)
{
    json j;

    Assimp::Importer importer;

    unsigned int flags = 0;
    flags |= aiProcess_CalcTangentSpace;
    flags |= aiProcess_Triangulate;
    flags |= aiProcess_GenNormals;
    flags |= aiProcess_JoinIdenticalVertices;
    flags |= aiProcess_SortByPType;
    flags |= aiProcess_FlipUVs;

    const aiScene *scene = importer.ReadFile(in, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading mesh: " << importer.GetErrorString() << "\n";
        exit(-1);
    }

    for (int i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh *mesh = scene->mMeshes[i];
        auto [path, name] = process_mesh(mesh, out);
        j["meshes"][i]["path"] = path;
        j["meshes"][i]["name"] = name;
    }

    if (scene->mNumLights > 0) {
        j["warnings"].push_back(
            "This scene appears to contain lights, this will cause some empty objects to be added "
            "with no componenets, please specify lighting manualy. Besides that lighting in assimp "
            "is somewhat broken with collada see https://github.com/assimp/assimp/issues/3399");
    }

    for (int i = 0; i < scene->mNumLights; i++) {
        // TODO
    }

    for (int i = 0; i < scene->mNumMaterials; i++) {
        const aiMaterial *mat = scene->mMaterials[i];

        j["materials"][i] = process_material(mat, scene);
    }

    for (auto &tex : textures) {
        static int i = 0;
        auto [key, val] = tex;

        j["textures"][i]["name"] = key;
        j["textures"][i++]["path"] = val;
    }

    aiNode *root_node = scene->mRootNode;

    j["scene"] = process_node(root_node, scene);

    std::cout << std::setw(2) << j << std::endl;
}