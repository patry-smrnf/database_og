#ifndef QUERY_H
#define QUERY_H
#include <string>
#include <unordered_map>
#include <vector>
#include "query_structures.h"
#include "../Databases/database.h"

//[!-- TO DO LIST --!]
//  [+] Czytac argumenty, rozrozniac je od komend
//  [ ] Stworzyc SELECT dla wielu tabel
//  [ ] Stworzyc obsluge WHERE
//  [ ] Dodac mozwliwosc oblusgi AND


static const std::unordered_map<std::string, tokens_types> token_map = {
    { "SELECT", tokens_types::SELECT },
    { "FROM",   tokens_types::FROM   },
    { "WHERE",  tokens_types::WHERE  }
};

class query_obj {
public:
    database db_user;
    explicit query_obj(const database& db): db_user(db) {}

    void exec(const std::string &RAWquery);
};


#endif //QUERY_H