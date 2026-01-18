#pragma once
#include <vector>

namespace client{
template<typename T>
struct Parser {
    std::function<tl::expected<T, client::ClientException> (size_t&, const std::vector<Token>&)> run;
    std::string label;

    tl::expected<T, client::ClientException> operator()(size_t& cursor, const std::vector<Token>& tokens) const {
        return run(cursor, tokens);
    };

    template<typename F>
    auto map(F f) const
        -> Parser<decltype(f(std::declval<T>()))>
    {
        using U = decltype(f(std::declval<T>()));
        auto selfRun = this->run;
        auto selfLabel = this->label;

        return Parser<U>{
            [=](size_t& pos, const std::vector<Token>& toks)
                -> tl::expected<U, client::ClientException>
            {
                auto r = selfRun(pos, toks);
                if (!r) return tl::unexpected(r.error());
                return f(*r);
            },
            label
        };
    }
};

template<typename A, typename B>
Parser<std::pair<A, B>> seq(Parser<A> p, Parser<B> q){
    return Parser<std::pair<A,B>>{
        [=](size_t& pos, const std::vector<Token>& toks) -> tl::expected<std::pair<A, B>, client::ClientException> {
            auto res1 = p(pos, toks);
            if (!res1) 
                return tl::unexpected(res1.error());

            auto res2 = q(pos, toks);
            if (!res2) 
                return tl::unexpected(res2.error());

            return std::pair{*res1, *res2};
        }
    };
};

template<typename T>
Parser<T> choice(const std::vector<Parser<T>> choices) {
    return Parser<T>{
        [=](size_t& pos, const std::vector<Token>& toks) -> tl::expected<T, client::ClientException> {
            for (auto candidate:choices){
                size_t save = pos;
                auto res = candidate(pos, toks);
                if (!res){
                    pos = save;
                    continue;
                };

                return *res;
            };

            std::string expected;
            for (size_t i = 0; i < choices.size(); ++i) {
                expected += choices[i].label;
                if (i + 1 < choices.size()) expected += " | ";
            }

            return tl::unexpected(client::ClientException(
                "Expected one of: " + expected + ", got " +
                (pos < toks.size()
                    ? utils::to_string(toks[pos].type)
                    : std::string("EOF"))
            ));
        }
    };
}

template<typename T>
Parser<std::vector<T>> many(Parser<T> p) {
    return Parser<std::vector<T>>{
        [=](size_t& pos, const std::vector<Token>& toks) -> tl::expected<std::vector<T>, client::ClientException> {
            std::vector<T> out;
            while(true){
                size_t save = pos;
                auto res = p(pos, toks);

                if (!res){
                    pos = save;
                    break;
                }

                out.push_back(*res);
            }

            return out;
        }
    };
}

template<typename A, typename B>
Parser<std::vector<A>> sep_by(Parser<A> a, Parser<B> b) {
    return Parser<std::vector<A>>{
        [=](size_t& pos, const std::vector<Token>& toks) -> tl::expected<std::vector<A>, client::ClientException> {
            std::vector<A> out;

            do{
                auto valRes = a(pos, toks);
                if (!valRes) 
                    return tl::unexpected(valRes.error());

                out.push_back(*valRes);

                size_t save = pos;
                auto sepRes = b(pos, toks);

                if (!sepRes){
                    pos = save;
                    break;
                }

            }while(true);

            return out;
        }
    };
}

template<typename T>
Parser<std::optional<T>> optional(Parser<T> p){
    return Parser<std::optional<T>>{
        [=](size_t& pos, const std::vector<Token>& toks) -> tl::expected<std::optional<T>, client::ClientException> {
            std::size_t save = pos;
            auto res = p(pos, toks);
            if (!res){
                pos = save;
                return std::optional<T>{};
            }
            return std::optional<T>{*res};
        }
    };
}

}