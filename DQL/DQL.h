#ifndef QUERY_H
#define QUERY_H
#include <string>
#include <unordered_map>
#include <vector>
#include "DQL_structures.h"
#include "../Databases/database.h"

//[!-- TO DO LIST --!]
//  [+] Czytac argumenty, rozrozniac je od komend
//  [+] Stworzyc SELECT dla wielu tabel
//  [+] Stworzyc obsluge WHERE
//  [+] Dodac mozwliwosc oblusgi AND


static const std::unordered_map<std::string, tokens_types> token_map = {
    { "SELECT", tokens_types::SELECT },
    { "FROM",   tokens_types::FROM   },
    { "WHERE",  tokens_types::WHERE  }
};

class DQL_obj {
public:
    database db_user;
    explicit DQL_obj(const database& db): db_user(db) {}

    void exec_dql(const std::string &RAWquery);
};


#endif //QUERY_H