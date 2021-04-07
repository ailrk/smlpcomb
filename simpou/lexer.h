#pragma once
#include <cstring>
#include <optional>
#include <string>
#include <variant>

template <typename T>
constexpr T
optchain_(std::optional<T> t) {
    return t.value();
}

template <typename T, typename... Args>
constexpr T
optchain_(std::optional<T> t, Args... args) {
    return t.value_or(optchain_(args...));
}

template <typename... Args>
constexpr auto
optchain(Args... args) -> std::optional<decltype(optchain_(args...))> {
    try {
        return optchain_(args...);
    } catch (std::bad_optional_access) {
        return {};
    }
}

enum TokenType {
    TokenEOF = -1,
    TokenDef = -2,
    TokenExtern = -3,
    TokenIdentifier = -4,
    TokenNumber = -5,
};

// lil token utill.
class Token {
    int value_;
    bool istok_ = false;

  public:
    Token(TokenType t)
        : value_(t)
        , istok_(true) {}

    Token(char c)
        : value_(c)
        , istok_(false) {}

    bool istok() const { return istok_; }

    std::optional<TokenType> token_type() {
        if (value_ >= -5 && value_ < 0) {
            return static_cast<TokenType>(value_);
        }
        return {};
    }

    int value() { return value_; }
};

class Lexer {
    char prev;

  public:
    std::string identifier_str;
    double n_val;

    Lexer()
        : identifier_str("")
        , n_val(0)
        , prev(' ') {}
    Token get_token();

  private:
    void skip_space() {
        while (isspace(prev)) {
            prev = getchar();
        }
    }

    std::optional<Token> keywords() {
        if (identifier_str == "def") {
            return TokenType::TokenDef;
        } else if (identifier_str == "extern") {
            return TokenType::TokenExtern;
        } else {
            return {};
        }
    }

    std::optional<Token> identifier() {
        if (isalnum(prev)) {
            identifier_str = prev;
            prev = getchar();
            while (isalnum(prev)) {
                identifier_str += prev;
                if (auto v = keywords()) {
                    return v.value();
                }
            }
            return TokenType::TokenIdentifier;
        }
        return {};
    }

    std::optional<Token> number() {
        if (isdigit(prev) || prev == '.') {
            std::string num;
            do {
                num += prev;
                prev = getchar();
            } while (isdigit(prev) || prev == '.');

            n_val = strtod(num.c_str(), nullptr);
            return TokenType::TokenNumber;
        }
        return {};
    }

    // parser till end of the line.
    std::optional<Token> comment() {
        if (prev == '#') {
            do {
                prev = getchar();
            } while (prev != EOF && prev != '\r' && prev != '\n');
        }
        return {};
    }

    std::optional<Token> eof() {
        if (prev == EOF) {
            return TokenType::TokenEOF;
        }
        return {};
    }
};

Token
Lexer::get_token() {
    skip_space();
    if (auto v = optchain(identifier(), number(), comment(), eof())) {
        return v.value();
    }

    char n = prev;
    prev = getchar();

    return n;
}
