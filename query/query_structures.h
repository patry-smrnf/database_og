#ifndef QUERY_STRUCTURES_H
#define QUERY_STRUCTURES_H
#include <string>
#include <vector>

#include "../Databases/database_structures.h"

enum class WHERE_OPERATORS {
    EQUAL, BIGGER_EQUAL, LOWER_EQUAL, DIFFERENT, LOWER, BIGGER, INVALID
};

struct WHERE_STATMNT_STRUCTURE {
    std::string value1;
    std::string value2;
    WHERE_OPERATORS operation;
};

struct SELECT_structure {
    std::string from;
    std::vector<std::string> data;
};

enum class tokens_types {
    SELECT, FROM, WHERE, INVALID
};

//[!-- Structura kazdej komendy w query, czyli oddzielny obiekt dla SELECT ..., FROM... itd --!]
struct command_structure {
    std::string name;
    std::vector<std::string> args;
};

#endif //QUERY_STRUCTURES_H
