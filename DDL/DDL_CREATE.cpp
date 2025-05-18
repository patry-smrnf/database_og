#include <fstream>

#include "DDL.h"
#include <fmt/base.h>

void DDL_obj::create(const std::vector<std::string> &ARGS_FIEL) {
    std::string title = ARGS_FIEL[1];

    auto [name, columns] = this->db_user.get_table_by_name(title);
    if (name != "INVALID") {
        fmt::print("[-] Wrong request, table '{}' already exists in yo database \n", title);
        exit(1);
    }

    bool fields_start = false;

    std::string key_name, key_type;

    std::string header_table_name = "["+title+"]";
    std::string header_columns = "{";
    for (auto i = 2; i < ARGS_FIEL.size(); i++) {
        if (ARGS_FIEL[i] == "(") {
            fields_start = true;
            continue;
        }
        if (ARGS_FIEL[i] == ")") {
            fields_start = false;
            break;
        }

        if (fields_start) {
            fmt::print("--{}\n--{}\n ", ARGS_FIEL[i], ARGS_FIEL[i+1]);
            key_name = ARGS_FIEL[i];
            key_type = ARGS_FIEL[i+1];
            if (!(key_type == "VARCHAR" || key_type == "INTEGER" || key_type == "TIMESTAMP")) {
                fmt::print("[-] Wrong request, '{}' theres no such a type \n", ARGS_FIEL[i+1]);
                exit(1);
            }
            header_columns += "(" + key_type + ")" + key_name;
            i++;
        }
    }
    header_columns += "}";

    std::ofstream database_file(this->db_user.path, std::ios::app);
    database_file << header_table_name << std::endl;
    database_file << header_columns << std::endl;
    fmt::print("[+] Table {} created\n", title);
    exit(0);
}
