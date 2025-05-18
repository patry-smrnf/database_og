#ifndef DDL_H
#define DDL_H
#include <string>
#include <unordered_map>
#include <vector>

#include "DDL_structures.h"
#include "../Databases/database.h"


class DDL_obj {
public:
    database db_user;
    explicit DDL_obj(const database& db): db_user(db) {}
    void exec_ddl(const std::string &RAWquery);
private:
    void create(const std::vector<std::string>& ARGS_FIEL);
    void drop(const std::vector<std::string>& ARGS_FIEL);
    static tokens_types_DDL convert_to_operator(const std::string &operator_);

};

#endif //DDL_H
