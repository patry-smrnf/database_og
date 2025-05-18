#include <fstream>
#include "DDL.h"
#include <fmt/base.h>

void DDL_obj::drop(const std::vector<std::string>& ARGS_FIEL) {
    std::string table_name = ARGS_FIEL[1];

    std::ofstream out_file(this->db_user.path + "temp", std::ios::app);
    bool found = false;
    for (auto i : this->db_user.tables) {
        if (i.name != table_name) {
            std::string HEADER_columns = "{";
            std::string HEADER_name = "[" + i.name + "]";


            for (auto kolumna : i.columns) {
                HEADER_columns += "(" + kolumna.type.type + ")" + kolumna.type.name;
            }
            HEADER_columns += "}";

            out_file << HEADER_name << std::endl;
            out_file << HEADER_columns << std::endl;

            for (auto index = 0; index < i.columns[0].values.size(); index++) {
                std::string values = "";
                for (auto value = 0; value < i.columns.size(); value++) {
                    values += i.columns[value].values[index] + ",";
                }
                values.pop_back();
                out_file << values << std::endl;
            }
            found = true;
        }
    }
    if (!found) {
        fmt::print("[-] Wrong DROP query, theres no {} table to drop \n", table_name);
        exit(1);
    }


    out_file.close();

    if (std::remove(this->db_user.path.c_str()) != 0) {
        fmt::print("[-] Something went wrong \n");
        exit(1);
    }

    if (std::rename((this->db_user.path + "temp").c_str(), this->db_user.path.c_str()) != 0) {
        fmt::print("[-] Something went wrong \n");
        exit(1);
    }

    fmt::print("[+] Table {} dropped", table_name);
    exit(0);
}
