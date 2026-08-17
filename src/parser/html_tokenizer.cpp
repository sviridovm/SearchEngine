#include "parser/html_tokenizer.hpp"
#include "parser/HtmlTags.h"


std::optional<HtmlTag> HtmlTokenizer::next_tag()
{
    cursor_.skip_until('<');
    if (cursor_.empty())
        return std::nullopt;

    cursor_.advance(); // '<'

    HtmlTag tag;

    if (cursor_.peek() == '/') {
        tag.closing = true;
        cursor_.advance();
    }

    cursor_.skip_whitespace();

    const auto name_start = cursor_.position();

    while (!cursor_.empty() &&
           !std::isspace(
               static_cast<unsigned char>(cursor_.peek())) &&
           cursor_.peek() != '>' &&
           cursor_.peek() != '/') {
        cursor_.advance();
    }

    tag.name = /* substring from name_start */;

    // parse attributes...

    return tag;
}