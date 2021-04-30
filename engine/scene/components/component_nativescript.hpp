#pragma once

#include <memory>

#include "core/native_script.hpp"

namespace blood
{

struct component_nativescript
{

    native_script *(*create_instance)();
    void (*destroy_script)(component_nativescript *);

    template <typename T>
    void bind()
    {
        create_instance = []() { return static_cast<native_script *>(new T()); };
        destroy_script = [](component_nativescript *nsc) {
            delete nsc->script;
            nsc->script = nullptr;
        };
    }

    native_script *script = nullptr;
};

} // namespace blood
