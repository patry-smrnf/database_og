#ifndef QUERY_STRUCTURES_H
#define QUERY_STRUCTURES_H
#include <string>
#include <vector>

#include "../Databases/database_structures.h"



enum class tokens_types {
    SELECT, FROM, WHERE, INVALID
};

//[!-- Structura kazdej komendy w DQL, czyli oddzielny obiekt dla SELECT ..., FROM... itd --!]
struct command_structure {
    std::string name;
    std::vector<std::string> args;
};

#endif //QUERY_STRUCTURES_H
