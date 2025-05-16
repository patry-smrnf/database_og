#include "query.h"

#include <algorithm>
#include <charconv>
#include <map>
#include <sstream>

#include "fmt/printf.h"

static std::string to_upper(const std::string &s) {
    std::string u; u.reserve(s.size());
    for (unsigned char c : s) u.push_back(std::toupper(c));
    return u;
}

bool token_detector(const std::string &word) {
    return token_map.count(to_upper(word)) > 0;
}

//ukradzione z neta, mozliwe ze chatgpt
bool is_integer(const std::string& s) {
    if (s.empty()) return false;
    size_t i = 0;
    if (s[0] == '+' || s[0] == '-') i = 1;
    if (i == s.size()) return false;
    for (; i < s.size(); ++i) {
        if (!std::isdigit(s[i])) return false;
    }
    return true;
}

WHERE_STATMNT_STRUCTURE parse_where(std::string val1, std::string val2, std::string val3) {
    bool found = false;
    WHERE_STATMNT_STRUCTURE out_structure;

    out_structure.value1 = val1;
    out_structure.value2 = val3;
    out_structure.operation = WHERE_OPERATORS::INVALID;

    static const std::map<std::string, WHERE_OPERATORS> op_map = {
        {"=",  WHERE_OPERATORS::EQUAL},
        {">",  WHERE_OPERATORS::BIGGER},
        {">=", WHERE_OPERATORS::BIGGER_EQUAL},
        {"<=", WHERE_OPERATORS::LOWER_EQUAL},
        {"<",  WHERE_OPERATORS::LOWER},
        {"!=", WHERE_OPERATORS::DIFFERENT},
    };

    const auto it = op_map.find(val2);
    if (it == op_map.end()) {
        fmt::print("[-] Invalid WHERE‑operator '{}'\n", val2);
        std::exit(1);
    }
    out_structure.operation = it->second;

    return out_structure;
}

//kazda query musi zaczynac sie od SELECT wiec po wykryciu select sprawdzam czy
void query_obj::exec(const std::string &RAWquery) {
    //[!-- Tymczasowe zmienne, niezbedne do obslugi --!]
    std::istringstream stream(RAWquery);
    std::string token;

    //[!-- Args fields dla kazdej komendy --!]
    std::vector<std::string> SELECT_args_FIELD;
    std::vector<std::string> FROM_args_FIELD;
    std::vector<std::string> WHERE_args_FIELD;

    std::vector<std::pair<std::string,std::string>> select_list;

    //[!-- Command field TEMP --!]
    std::string TEMP_actual_command = "";

    //[!-- Przelatuje wszystkie slowa pokolej --!]
    while (stream >> token) {
        std::erase(token, ','); //[!-- Usuwam wszystkie przecinki, zbedne sa --!]
        if (token_detector(token)) { //[!-- Sprawdzam czy jest jakis keyword --!]
            TEMP_actual_command = token;
        }
        else { //[!-- Jesli nie keyword, to traktuje to jako argument --!]
            if (TEMP_actual_command == "SELECT") {
                SELECT_args_FIELD.push_back(token);
            }
            else if (TEMP_actual_command == "FROM") {
                FROM_args_FIELD.push_back(token);
            }
            else if (TEMP_actual_command == "WHERE") {
                WHERE_args_FIELD.push_back(token);
            }
            else {
                fmt::print(stderr, "Unknown token {}\n", TEMP_actual_command);
            }
        }
    }

    //[!-- Analyze FROM --!]
    bool more_than_one_TABLE = FROM_args_FIELD.size() > 1;

    //[!-- Sprawdzam czy istnieja podane tablice w bazie
    for (const auto& given_from_query : FROM_args_FIELD) {
        bool match_found = false;

        auto [name, columns] = this->db_user.get_table_by_name(given_from_query);
        if (name != "INVALID") {
            match_found = true;
            break;
        }

        if (!match_found) {
            fmt::print("[-] Wrong request, there's no table '{}' in your database\n", given_from_query);
            exit(1);
        }
    }

    //[!-- Analyze SELECT --!]
    //[!-- Jesli import z wiecej niz jedna tablica --!]
    if (more_than_one_TABLE) {
        for (const auto& select_arg : SELECT_args_FIELD) {
            if (select_arg.find('.') != std::string::npos) {
                auto dotPos = select_arg.find('.');
                auto data = select_arg.substr(0, dotPos);
                auto table = select_arg.substr(dotPos + 1);

                //[!-- Sprawdzam czy istnieje taka tablica w importowanych tablicach --!]
                bool does_table_exist = false;
                for (const auto& FROM_args : FROM_args_FIELD) {
                    if (FROM_args == table) { does_table_exist = true; }
                }
                if (does_table_exist) {
                    //[!-- Teraz nalezy sprawdzic czy taka kolumna istnieje w bazie --!]
                    if (!this->db_user.does_column_exists(this->db_user.get_table_by_name(table), data)) {
                        fmt::print("[-] Wrong request, theres no '{}' in your database", data);
                        exit(1);
                    }
                    select_list.emplace_back(table, data);
                }
                else {
                    fmt::print("[-] Wrong request, make sure u wrote correct table name - example of bad usage: SELECT username.users, type.idunno FROM users, config");
                    exit(1);
                    break;
                }
            }
            else {
                fmt::print("[-] Wrong request, you are importing from more than one table but no reference in imported data - example: SELECT username.users, type.config FROM users, config");
                exit(1);
                break;
            }
        }
    }
    else {
        for (const auto& select_args : SELECT_args_FIELD) {
            //[!-- Teraz nalezy sprawdzic czy taka kolumna istnieje w bazie --!]
            if (!this->db_user.does_column_exists(this->db_user.get_table_by_name(FROM_args_FIELD[0]), select_args)) {
                fmt::print("[-] Wrong request, theres no '{}' in your database", select_args);
                exit(1);
            }
            select_list.emplace_back(FROM_args_FIELD[0], select_args);
        }
    }

    //[!-- Analyze WHERE --!]
    if (WHERE_args_FIELD.empty()) {
        for (auto map_select : select_list) {
            fmt::print("[{}.{}] \n", map_select.first, map_select.second);
            for (auto kolumna : this->db_user.get_column_values_by_name(this->db_user.get_table_by_name(map_select.first), map_select.second)) {
                fmt::print("{}\n", kolumna);
            }
        }
    }
    else {

    }

}
