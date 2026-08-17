#pragma once

#include <cctype>
#include <string_view>


class HtmlCursor {
public:
    explicit HtmlCursor(std::string_view input)
        : input_(input) {}

    [[nodiscard]]
    bool empty() const noexcept {
        return position_ >= input_.size();
    }

    [[nodiscard]]
    char peek() const noexcept {
        return empty() ? '\0' : input_[position_];
    }

    [[nodiscard]]
    char peek(std::size_t offset) const noexcept {
        const auto index = position_ + offset;
        return index < input_.size() ? input_[index] : '\0';
    }

    char advance() noexcept {
        return empty() ? '\0' : input_[position_++];
    }

    void skip_whitespace() noexcept {
        while (!empty() &&
               std::isspace(static_cast<unsigned char>(peek()))) {
            advance();
        }
    }

    [[nodiscard]]
    std::string_view remaining() const noexcept {
        return input_.substr(position_);
    }

    [[nodiscard]]
    std::size_t position() const noexcept {
        return position_;
    }

    void advance(std::size_t count) noexcept {
        position_ = std::min(position_ + count, input_.size());
    }

    bool consume(char c) noexcept {
      if (peek() != c)
         return false;
      advance();
      return true;
   }

    void skip_until(char target) noexcept {
      while (!empty() && peek() != target)
      {
         advance();
      }
   }

private:
    std::string_view input_;
    std::size_t position_ = 0;
};