#pragma once

#include <string>

class modal
{
public:
    modal(const std::string &title, const std::string &text, std::string *input = nullptr);

    void show();

    bool update();

    ~modal();

public:
    bool m_show = false;

    std::string m_title;
    std::string m_text;
    std::string *m_input;
};