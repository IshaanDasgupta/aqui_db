#include "client/token.hpp"
#include "client/lexer.hpp"
#include <re2/re2.h>
#include <array>
#include <regex>
#include <tl/expected.hpp>
#include <client/exception.hpp>
#include <iostream>
#include <utils/to_string.hpp>

namespace client {

struct TokenSpec {
    TokenType type;
    std::string pattern;
};

static const std::vector<TokenSpec> tokenSpecs = {
#define X(name, regex) TokenSpec{TokenType::name, std::string("^(") + regex + std::string(")")},
#include "token.def"
#undef X
};

static const std::unordered_map<std::string, TokenType> keywordLookup = {
#define K(name, value) {value, TokenType::name},
#include "keyword.def"
#undef K
};


tl::expected<std::vector<client::Token>, client::ClientException> Lexer::tokenize(const std::string &cargo) {
    std::vector<Token> tokens;
    size_t cursor = 0;
    
    while(cargo[cursor] != '\0'){
        TokenType matchedTokenType = TokenType::UNKNOWN;
        re2::StringPiece longestMatch("");

        for (const TokenSpec& spec : tokenSpecs){
            //RE2::Consume advances the input cursor furthur hence we need to create a stringPiece for each match
            re2::StringPiece inputAtCursor(cargo.data() + cursor, cargo.size() - cursor);
            re2::RE2 regex(spec.pattern);
            re2::StringPiece match;
            
            if (RE2::Consume(&inputAtCursor, regex, &match)) {
            if (match.size() > longestMatch.size()) {
                    longestMatch = match;
                    matchedTokenType = spec.type;
                }
            }
        }

        if (matchedTokenType == TokenType::UNKNOWN) {
            return tl::unexpected(ClientException("No match found at position " + std::to_string(cursor)));
        }

        if (matchedTokenType == TokenType::TK_WHITESPACE) {
            cursor += longestMatch.size();
            continue;
        }

        std::string tokenValue = std::string(longestMatch.data(), longestMatch.size());
        std::string lower = tokenValue;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return std::tolower(c); });
        std::unordered_map<std::string, TokenType>::const_iterator it = keywordLookup.find(lower);

        //upgrade indent to keyword if possible
        if (matchedTokenType == TokenType::TK_IDENT && it != keywordLookup.end()){
            matchedTokenType = (*it).second;
            tokenValue = lower;
        }

        tokens.push_back(Token{matchedTokenType, tokenValue});
        cursor += longestMatch.size();
    };

    return tokens;
}

}
