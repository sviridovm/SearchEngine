#pragma once

#include <optional>
#include "parser/html_cursor.hpp"
#include "parser/HtmlTags.h"

class HtmlTokenizer {
public:
    explicit HtmlTokenizer(std::string_view html);

    std::optional<HtmlTag> next_tag();
    std::optional<std::string_view> next_text();

private:
    HtmlCursor cursor_;
};