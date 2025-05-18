//
// Created by patry on 18/05/2025.
//
#include <regex>
#include <sstream>
#include <fmt/base.h>
#include "DML.h"

#include <fstream>

void DML_obj::exec_dml(const std::string &RAWquery) {
    //[!-- Args fields dla kazdej komendy --!]
    std::vector<std::string> ALL_args;

    //[!-- Przelatuje wszystkie slowa pokolej --!]
    std::regex token_regex(R"((\'[^\']*\'|\S+))");
    auto words_begin = std::sregex_iterator(RAWquery.begin(), RAWquery.end(), token_regex);
    auto words_end = std::sregex_iterator();

    for (auto it = words_begin; it != words_end; ++it) {
        std::string token = it->str();
        std::erase(token, ',');
        ALL_args.push_back(token);
    }


    if (ALL_args[0] != "INSERT") {
        fmt::print("[-] Wrong request, DML request has to have 'INSERT' ");
        exit(1);
    }

    std::string table_name = ALL_args[1];

    if (ALL_args[2] != "VALUES") {
        fmt::print("[-] Wrong request, DML request has to have 'VALUES' ");
        exit(1);
    }

    if (this->db_user.get_table_by_name(table_name).name == "INVALID") {
        fmt::print("[-] Wrong request, DML theres no table {} in yo database", table_name);
        exit(1);
    }

    std::vector<std::vector<std::string>> wartosci_do_wpisania;
    int ilosc_kolumn_dla_tablicy = this->db_user.get_table_by_name(table_name).columns.size();
    bool values_row = false;
    int counter_helper = 0;

    std::vector<std::string> row;
    for (auto values = 3; values < ALL_args.size(); values++) {
        if (counter_helper == ilosc_kolumn_dla_tablicy+1 && values_row) {
            fmt::print("[-] Wrong request, too much values for this row ");
            exit(1);
        }

        if (ALL_args[values] == "(") {
            values_row = true;
            continue;
        }
        if (ALL_args[values] == ")") {
            if (counter_helper != ilosc_kolumn_dla_tablicy) {
                fmt::print("[-] Wrong request, not proper values count for this row ");
                exit(1);
            }
            values_row = false;
            counter_helper =0;
            wartosci_do_wpisania.push_back(row);
            row.clear();
            continue;
        }

        if (values_row) {
            row.push_back(ALL_args[values]);
            counter_helper++;
        }
    }



    std::string HEADER_EDITED_columns = "{";
    std::string HEADER_EDITED_name = "[" + table_name + "]";

    std::ofstream out_file(this->db_user.path + "temp", std::ios::app);
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
        }
        else {
            for (auto kolumna : i.columns) {
                HEADER_EDITED_columns += "(" + kolumna.type.type + ")" + kolumna.type.name;
            }
            HEADER_EDITED_columns += "}";

            std::vector<std::string> wartosci;

            for (auto index = 0; index < i.columns[0].values.size(); index++) {
                for (auto value = 0; value < i.columns.size(); value++) {
                    std::string temp = i.columns[value].values[index];
                    if (temp[0] == ' ') {
                        temp.erase(0, 1);
                    }
                    wartosci.push_back(temp);
                }
                wartosci_do_wpisania.push_back(wartosci);
                wartosci.clear();
            }
        }
    }

    out_file << HEADER_EDITED_name << std::endl;
    out_file << HEADER_EDITED_columns << std::endl;
    std::string temp_to_write = "";
    for (auto i : wartosci_do_wpisania) {
        for (auto y : i) {
            temp_to_write += y + ",";
        }
        temp_to_write.pop_back();
        out_file << temp_to_write << std::endl;
        temp_to_write = "";
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

    fmt::print("[+] Values succesfully inserted\n");

    exit(0);
}
