#include "query.h"
#include <algorithm>
#include <charconv>
#include "Misc/Where_STATEMENT.h"
#include <sstream>

#include "fmt/printf.h"

static std::string to_upper(const std::string &s) {
    std::string u; u.reserve(s.size());
    for (unsigned char c : s) u.push_back(std::toupper(c));
    return u;
}

bool token_detector(const std::string &word) {
    return token_map.contains(to_upper(word));
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

//kazda query musi zaczynac sie od SELECT wiec po wykryciu select sprawdzam czy
void query_obj::exec(const std::string &RAWquery) {
    //[!-- Tymczasowe zmienne, niezbedne do obslugi --!]
    std::istringstream stream(RAWquery);
    std::string token;

    //[!-- Args fields dla kazdej komendy --!]
    std::vector<std::string> SELECT_args_FIELD;
    std::vector<std::string> FROM_args_FIELD;
    std::vector<std::string> WHERE_args_FIELD;

    //[!-- Konstrukcja table_name : column_name
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
    auto more_than_one_TABLE = FROM_args_FIELD.size() > 1;

    //[!-- Sprawdzam czy istnieja podane tablice w bazie
    for (const auto& given_from_query : FROM_args_FIELD) {
        auto match_found = false;

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
        for (auto i : SELECT_args_FIELD) { //[!-- NIE POZWALAM NA WYSWIETLENI WSYZTKIEGO Z PARU TABLIC --!]
            if (i == "*") {
                fmt::print("[-] Wrong request, using '*' not allowed when u import from more than one table\n");
                exit(1);
            }
        }
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
        if (SELECT_args_FIELD[0] == "*") {
            if (SELECT_args_FIELD.size() == 1) {
                database_methods::print_table(this->db_user.get_table_by_name(FROM_args_FIELD[0]));
            }
            else {
                fmt::print("[-] Wrong request, using '*' not allowed when u import from more than one data\n");
                exit(1);
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
    }

    //[!-- Analyze WHERE --!]
    if (WHERE_args_FIELD.empty()) { // [!-- Jesli nie ma warunkow --!]
        database_methods::print_mixed_tables(select_list, this->db_user.tables);
    }
    else {
        std::vector<std::vector<std::string>> where_statements; // <-- Tablica gdzie beda przechowane wszystkie warunki przekazane przez uzytkownika
        std::vector<std::string> logic_operators; // <-- Liczba operatorow logicznych musi byc -1 mniejsza od warunkow

        //[!-- Pomysl3, tworzyc nowy obiekt tablicy tylko dla data spelniajaca warunek

        //[!-- To powinno stworzyc wektor z listami warunkow oraz wektor z operatorami logicznymi
        std::vector<std::string> TEMP_where_statement;
        for (auto i = 0; i < WHERE_args_FIELD.size(); i++) {
            if (WHERE_args_FIELD[i] != "OR" && WHERE_args_FIELD[i] != "AND") {
                TEMP_where_statement.push_back(WHERE_args_FIELD[i]);
            }
            else {
                if (TEMP_where_statement.size() == 3) {
                    where_statements.push_back(TEMP_where_statement);
                    TEMP_where_statement.clear();

                    logic_operators.push_back(WHERE_args_FIELD[i]);
                }
                else {
                    fmt::print("[-] Wrong WHERE statement, there must be 3 values in the WHERE statement \n");
                    exit(1);
                }
            }
        }
        if (!TEMP_where_statement.empty()) {
            where_statements.push_back(TEMP_where_statement);
        }

        //[!-- Walidacja poprawnosci warunku WHERE --!]
        if (logic_operators.size()+1 != WHERE_args_FIELD.size() && logic_operators.size() != 0) {
            fmt::print("[-] Wrong WHERE statement, there must be two conditions for one operator. For example: WHERE username = 'admin' AND id = 0 \n");
            exit(1);
        }

        //[!-- Parsowanie --!]
        std::vector<WHERE_struct> PARSED_where_statements;
        std::vector<LOGIC_where> PARSED_logic_operators;

        //  [--! 1. Parsowanie warunkow WHERE --!]
        for (auto i : where_statements) {
            WHERE_struct temp_where;
            temp_where.val1 = i[0];
            temp_where.val2 = i[2];
            temp_where.operatorWhere = Where_STATEMENT::convert_to_operator(i[1]);

            PARSED_where_statements.push_back(temp_where);
        }

        //  [--! 2. Parsowanie operatorow logicznych WHERE --!]
        for (auto i : logic_operators) {
            if (i == "OR") { PARSED_logic_operators.push_back(LOGIC_where::OR); }
            if (i == "AND") { PARSED_logic_operators.push_back(LOGIC_where::AND); }
        }

        //[!-- ciezkie, Tworzenie nowych tablic spelniajacych warunki --!]
        Table output_table;
        std::vector<Column>kolumny_do_pobrania;

        for (auto i : PARSED_where_statements) {
            //[!-- W tym jezyku pierwsza wartosc musi byc odniesieniem sie do kolumny w bazie danych
            std::vector<std::string> values_of_val1; //<-- Tutaj beda ladowane wartosci kolumny pierwszej

            std::vector<std::string> correct_values;

            Column temp_column_out;

            temp_column_out.type.name = i.val1 + " " + i.val2;

            if (more_than_one_TABLE) {
                auto dotPos = i.val1.find('.');
                auto column_name = i.val1.substr(0, dotPos);
                auto table_name = i.val1.substr(dotPos + 1);

                values_of_val1 = this->db_user.get_column_values_by_name(this->db_user.get_table_by_name(table_name), column_name);
            }
            else {
                values_of_val1 = this->db_user.get_column_values_by_name(this->db_user.get_table_by_name(FROM_args_FIELD[0]), i.val1);
            }

            switch (i.operatorWhere) {
                case WHERE_operator::EQUAL:
                    std::erase(i.val2, '\'');

                    for (auto values : values_of_val1) {
                        if (values == i.val2) {
                            fmt::print("!!! {}", values);
                            correct_values.push_back(values);
                        }
                    }
                    break;
                case WHERE_operator::NOT:
                    std::erase(i.val1, '\'');
                    for (auto values : values_of_val1) {
                        if (values != i.val2) {
                            correct_values.push_back(values);
                        }
                    }
                    break;

                case WHERE_operator::LOWER:
                    break;
                default:
                    fmt::print("[-] Unknown error \n");
                    exit(1);
            }

            temp_column_out.values = correct_values;
            kolumny_do_pobrania.push_back(temp_column_out);

        }

        output_table.columns = kolumny_do_pobrania;
        database_methods::print_table(output_table);


    }

}
