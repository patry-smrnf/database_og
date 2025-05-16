//
// Created by patry on 16/05/2025.
//

#include "Where_STATEMENT.h"

#include <unordered_map>

#include "fmt/xchar.h"

static const std::unordered_map<std::string, WHERE_operator> operator_map = {
    { "!=", WHERE_operator::NOT },
    { "=",   WHERE_operator::EQUAL   },
    { "<",   WHERE_operator::LOWER   },
    { ">",   WHERE_operator::BIGGER   },
    { ">=",   WHERE_operator::BIGGER_EQAL   },
    { "<=",  WHERE_operator::LOWER_EQUAL  }
};

WHERE_operator Where_STATEMENT::convert_to_operator(const std::string &operator_) {
    if (auto it = operator_map.find(operator_); it != operator_map.end()) {
        return it->second;
    } else {
        return WHERE_operator::INVALID;
    }
}

bool Where_STATEMENT::is_number(const std::string& value) {

    double test;
    const auto begin= value.data();
    const auto end= value.data() + value.size();

    auto [ptr, ec] = std::from_chars(begin, end, test);

    return ec == std::errc() && ptr == end;
}

bool Where_STATEMENT::is_string(const std::string& value) {
    if (value[0] == '\'' && value[value.size() - 1] =='\'') {
        return true;
    }
    return false;
}