#ifndef DML_H
#define DML_H
#include <string>
#include <unordered_map>
#include <vector>
#include "../Databases/database.h"

class DML_obj {
public:
    database db_user;
    explicit DML_obj(const database& db): db_user(db) {}

    void exec_dml(const std::string &RAWquery);
};



#endif //DML_H
