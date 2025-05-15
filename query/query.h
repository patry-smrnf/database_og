#ifndef QUERY_H
#define QUERY_H
#include <string>
#include <unordered_map>
#include <vector>

enum class tokens_types {
    SELECT, FROM, WHERE, INVALID
};

static const std::unordered_map<std::string, tokens_types> token_map = {
    { "SELECT", tokens_types::SELECT },
    { "FROM",   tokens_types::FROM   },
    { "WHERE",  tokens_types::WHERE  }
};

struct command {
    std::string name;
    std::vector<std::string> args;
};

class query_obj {
public:
    std::vector<command> commands;
};

class query_misc : public query_obj {
public:
    static void tokenize(const std::string &RAWquery);
};

#endif //QUERY_H