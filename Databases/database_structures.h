#ifndef DATABASE_STRUCTURES_H
#define DATABASE_STRUCTURES_H
#include <string>
#include <vector>

struct Type {
    std::string type;
    std::string name;
};

struct Column {
    Type type;
    std::vector<std::string> values;
};

struct Table {
    std::string name;
    std::vector<Column> columns;
};

#endif //DATABASE_STRUCTURES_H
