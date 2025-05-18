#include "DDL.h"

#include <regex>
#include <sstream>
#include <fmt/base.h>
#include "DDL_structures.h"

static const std::unordered_map<std::string, tokens_types_DDL> token_map = {
    { "CREATE", tokens_types_DDL::CREATE },
    { "ALTER", tokens_types_DDL::ALTER },
    { "DROP", tokens_types_DDL::DROP },
    { "RENAME", tokens_types_DDL::RENAME },
    { "TRUNCATE", tokens_types_DDL::TRUNCATE },
};

tokens_types_DDL DDL_obj::convert_to_operator(const std::string &operator_) {
    if (auto it = token_map.find(operator_); it != token_map.end()) {
        return it->second;
    } else {
        return tokens_types_DDL::INVALID;
    }
}

void DDL_obj::exec_ddl(const std::string &RAWquery) {
    //[!-- Args fields dla kazdej komendy --!]
    std::vector<std::string> ALL_args;

    //[!-- Przelatuje wszystkie slowa pokolej --!]
    std::regex token_regex(R"((\'[^\']*\'|\S+))");
    auto words_begin = std::sregex_iterator(RAWquery.begin(), RAWquery.end(), token_regex);
    auto words_end = std::sregex_iterator();

    for (auto it = words_begin; it != words_end; ++it) {
        std::string token = it->str();
        std::erase(token, ',');
        ALL_args.push_back(token);
    }

    switch (convert_to_operator(ALL_args[0]))
    {
        case tokens_types_DDL::CREATE:
            create(ALL_args);
            break;
        case tokens_types_DDL::DROP:
            drop(ALL_args);
            break;
        default:
            break;

    }

}
