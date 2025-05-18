#ifndef WHERE_STATEMENT_H
#define WHERE_STATEMENT_H
#include <string>

enum class WHERE_operator {
    EQUAL, NOT, BIGGER, LOWER, BIGGER_EQAL, LOWER_EQUAL, INVALID
};

struct WHERE_struct {
    std::string val1;
    std::string val2;
    WHERE_operator operatorWhere;
};

enum class LOGIC_where {
    OR, AND
};

class Where_STATEMENT {
    public:
    static WHERE_operator convert_to_operator(const std::string& operator_);
    static std::string operator_to_string(const WHERE_operator& operator_);
    static bool is_number(const std::string& value);
    static bool is_string(const std::string& value);
};



#endif //WHERE_STATEMENT_H
