#ifndef DATABASE_H
#define DATABASE_H
#include <string>

#include "database_structures.h"
#include "../Misc/app.h"

enum class data_types {
    INTEGER,
    VARCHAR,
    TIMESTAMP,
    INVALID
};

class database_creator {
public:
    static std::string create_table_hedaer(const std::string& name);
    static std::string create_types_header(std::vector<Type>& typy);
    static std::string create_values_header(std::vector<Type> typy, std::vector<std::string>& values);
};

class database_reader {
public:
    static bool is_it_header(const std::string& line);
    static bool is_it_types_row(std::string const line);

    static std::vector<Type> parse_fields(std::string const line);
    static std::vector<std::string> parse_values_row(std::string line);

    //[!-- Tworzy obiekt tablicy wykorzystujac wartosci i typy, pozyteczne w chuj --!]
    static Table make_it_table(std::string name, std::vector<Type> types, std::vector<std::vector<std::string>> values);

};

class database_methods {
public:
    static void print_table_details(Table& table);
};

class database {
public:
    app user_session;
    std::vector<Table> tables;
    database(const app &session);
    void new_database_setup();
    void load_database(const std::string &path);

    //[!-- Metody wykorzystywane przez obiekt --!]
    Table get_table_by_name(const std::string& name) const;
private:
    void tables_setup(std::ofstream& database_file, std::vector<Table>& tables);
};



#endif //DATABASE_H
