#include "material.hpp"
namespace fw {
material::material() {}

bool material::save() { return true; }

bool material::load_from_file(const std::string &vfs_path) { return true; }

material::~material() {}
} // namespace fw