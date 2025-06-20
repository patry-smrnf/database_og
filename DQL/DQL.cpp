#include "DQL.h"
#include <algorithm>
#include <charconv>
#include <regex>

#include "Misc/Where_STATEMENT.h"
#include <sstream>

#include "fmt/printf.h"

static std::vector<std::string> fix_my_values(std::vector<std::string> values)
{
    std::vector<std::string> result;
    for (auto i : values) {
        if (i[0] == ' ') {
            result.push_back(i.substr(1));
        }
        else {
            result.push_back(i);
        }
    }
    return result;
}

static std::string to_upper(const std::string &s) {
    std::string u; u.reserve(s.size());
    for (unsigned char c : s) u.push_back(std::toupper(c));
    return u;
}

bool token_detector(const std::string &word) {
    return token_map.contains(to_upper(word));
}

bool is_it_unique(std::vector<int>& array, int value) {
    for (auto i : array) {
        if (i == value) {
            return false;
        }
    }
    return true;
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

//kazda DQL musi zaczynac sie od SELECT wiec po wykryciu select sprawdzam czy
void DQL_obj::exec_dql(const std::string &RAWquery) {
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

    //[!-- fixed, wylapywanie spacji w slowach z rawQUERY
    std::regex token_regex(R"((\'[^\']*\'|\S+))");
    auto words_begin = std::sregex_iterator(RAWquery.begin(), RAWquery.end(), token_regex);
    auto words_end = std::sregex_iterator();

    for (auto it = words_begin; it != words_end; ++it) {
        std::string token = it->str();
        std::erase(token, ',');
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

    //[!-- Przelatuje wszystkie slowa pokolej --!]
    // while (stream >> token) {
    //     std::erase(token, ','); //[!-- Usuwam wszystkie przecinki, zbedne sa --!]
    //     if (token_detector(token)) { //[!-- Sprawdzam czy jest jakis keyword --!]
    //         TEMP_actual_command = token;
    //     }
    //     else { //[!-- Jesli nie keyword, to traktuje to jako argument --!]
    //         fmt::print("- {}\n", token);
    //         if (TEMP_actual_command == "SELECT") {
    //             SELECT_args_FIELD.push_back(token);
    //         }
    //         else if (TEMP_actual_command == "FROM") {
    //             FROM_args_FIELD.push_back(token);
    //         }
    //         else if (TEMP_actual_command == "WHERE") {
    //             WHERE_args_FIELD.push_back(token);
    //         }
    //         else {
    //             fmt::print(stderr, "Unknown token {}\n", TEMP_actual_command);
    //         }
    //     }
    // }

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
                for (auto kolumny : this->db_user.get_table_by_name(FROM_args_FIELD[0]).columns) {
                    select_list.emplace_back(FROM_args_FIELD[0], kolumny.type.name);
                }
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
        if (logic_operators.size()+1 != WHERE_args_FIELD.size()/3 && logic_operators.size() != 0) {
            fmt::print("[-] {} \n Wrong WHERE statement, there must be two conditions for one operator. For example: WHERE username = 'admin' AND id = 0 \n", WHERE_args_FIELD.size()/3);
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
        output_table.name = "WHERE OUTPUT";
        std::vector<Column>kolumny_do_pobrania; // [!-- Kolumny do outputowej tabeli ktora bedzie printowana

        std::vector<std::vector<int>> poprawne_indexy_wynikow; // [!-- kazda operacja zwroci indexy elementow spelniajacych warunek w postaci wektora, zewnetrzeny wektor ma przechowywac te wektory by pote
        //potem ustalac wyniki AND lub OR

        std::vector<Column> rest_of_columns; // <-- reszta kolumn ktore zostaly selectowane przez usera
        for (const auto& selected : select_list) { // <-- Przypisywanie wartosci do kolumn
            rest_of_columns.push_back(this->db_user.get_column(this->db_user.get_table_by_name(selected.first), selected.second));
        }

        //[!-- Przelatywanie po wszystkich warunkach
        for (auto i : PARSED_where_statements) {
            //[!-- W tym jezyku pierwsza wartosc musi byc odniesieniem sie do kolumny w bazie danych

            std::vector<std::string> values_of_val1; //<-- Tutaj beda ladowane wartosci kolumny pierwszej

            //Dane na ktorych beda robione operacje
            if (more_than_one_TABLE) {
                fmt::print("[-] Wrong WHERE statement, u cant do WHERE operations when theres more than one table imported \n");
                exit(1);
            }
            else {
                if (i.val1.find('.') != std::string::npos) {
                    fmt::print("[-] Wrong WHERE statement, no need to precise which table if you import only one \n");
                    exit(1);
                }
                else {
                    values_of_val1 = fix_my_values(this->db_user.get_column_values_by_name(this->db_user.get_table_by_name(FROM_args_FIELD[0]), i.val1));
                }
            }

            std::vector<int> correct_indexes; // <-- Poprawne indexy, spelniajace warunki - ale tylko dla obecnie wykonywanej operacji

            //[!-- Wykonywanie operacji logicznych na operatorach i wartosciach
            switch (i.operatorWhere) {
                case WHERE_operator::EQUAL:
                    std::erase(i.val2, '\'');

                    for (auto count = 0 ; count < values_of_val1.size(); count++) {
                        if (values_of_val1[count] == i.val2) {
                            correct_indexes.push_back(count); //[!-- Zapisanie poprawnych indexow spelniajacych warunek
                        }
                    }

                    break;
                case WHERE_operator::NOT:
                    std::erase(i.val2, '\'');
                    for (auto count = 0 ; count < values_of_val1.size(); count++) {
                        if (values_of_val1[count] != i.val2) {
                            correct_indexes.push_back(count); //[!-- Zapisanie poprawnych indexow spelniajacych warunek
                        }
                    }
                    break;
                case WHERE_operator::LOWER:
                    if (is_integer(i.val2)) {
                        int val2 = std::stoi(i.val2);
                        for (auto count = 0; count < values_of_val1.size(); count++) {
                            if (is_integer(values_of_val1[count])) {
                                if (std::stoi(values_of_val1[count]) < val2) {
                                    correct_indexes.push_back(count);
                                }
                            }
                            else {
                                fmt::print("[-] Wrong WHERE statement, the data from table and column u picked are not an integers \n");
                                exit(1);
                            }
                        }
                    }
                    else {
                        fmt::print("[-] Wrong WHERE statement, the second value is not an integer \n");
                        exit(1);
                    }
                    break;
                case WHERE_operator::BIGGER:
                    if (is_integer(i.val2)) {
                        int val2 = std::stoi(i.val2);
                        for (auto count = 0; count < values_of_val1.size(); count++) {
                            if (is_integer(values_of_val1[count])) {
                                if (std::stoi(values_of_val1[count]) > val2) {
                                    correct_indexes.push_back(count);
                                }
                            }
                            else {
                                fmt::print("[-] Wrong WHERE statement, the data from table and column u picked are not an integers \n");
                                exit(1);
                            }
                        }
                    }
                    else {
                        fmt::print("[-] Wrong WHERE statement, the second value is not an integer \n");
                        exit(1);
                    }
                    break;
                case WHERE_operator::LOWER_EQUAL:
                    if (is_integer(i.val2)) {
                        int val2 = std::stoi(i.val2);
                        for (auto count = 0; count < values_of_val1.size(); count++) {
                            if (is_integer(values_of_val1[count])) {
                                if (std::stoi(values_of_val1[count]) <= val2) {
                                    correct_indexes.push_back(count);
                                }
                            }
                            else {
                                fmt::print("[-] Wrong WHERE statement, the data from table and column u picked are not an integers \n");
                                exit(1);
                            }
                        }
                    }
                    else {
                        fmt::print("[-] Wrong WHERE statement, the second value is not an integer \n");
                        exit(1);
                    }
                    break;
                case WHERE_operator::BIGGER_EQAL:
                    if (is_integer(i.val2)) {
                        int val2 = std::stoi(i.val2);
                        for (auto count = 0; count < values_of_val1.size(); count++) {
                            if (is_integer(values_of_val1[count])) {
                                if (std::stoi(values_of_val1[count]) >= val2) {
                                    correct_indexes.push_back(count);
                                }
                            }
                            else {
                                fmt::print("[-] Wrong WHERE statement, the data from table and column u picked are not an integers \n");
                                exit(1);
                            }
                        }
                    }
                    else {
                        fmt::print("[-] Wrong WHERE statement, the second value is not an integer \n");
                        exit(1);
                    }
                    break;
                default:
                    fmt::print("[-] Unknown error \n");
                    exit(1);
            }

            poprawne_indexy_wynikow.push_back(correct_indexes); //[!-- Dodanie wektora z indexami dla tej operacji do glownego wektora laczacego kazda operacje
        }

        //[!-- Powazna duza zabawa z operatorami AND OR
        std::vector<int> spelniajaceANDiOR;
        //[!-- Sprawdzam czy wogole isnieje and lub or
        bool czy_and_or = false;
        for (auto i=0; i < PARSED_logic_operators.size(); i++) {
            if (PARSED_logic_operators[i] == LOGIC_where::OR || PARSED_logic_operators[i] == LOGIC_where::AND) {
                czy_and_or = true;
            }
        }

        if (czy_and_or && poprawne_indexy_wynikow.size() != 1) {
            //[!-- Wpierw wykonanie wszystkiego zwiazanego z OR
            std::vector<int>spelniajace_OR;
            for (auto i=0; i < PARSED_logic_operators.size(); i++) {
                if (PARSED_logic_operators[i] == LOGIC_where::OR) {
                    fmt::print("lewa Strona \n");

                    //Warunek po lewej stronie OR
                    for (auto indexy : poprawne_indexy_wynikow[i]) {
                        fmt::print("---{} \n", indexy);
                        if (is_it_unique(spelniajace_OR, indexy)) {
                            spelniajace_OR.push_back(indexy);
                        }
                    }
                    fmt::print("Prawa Strona \n");

                    //Warunek po prawej stronie OR
                    for (auto indexy : poprawne_indexy_wynikow[i+1]) {
                        fmt::print("---{} \n", indexy);
                        if (is_it_unique(spelniajace_OR, indexy)) {
                            spelniajace_OR.push_back(indexy);
                        }
                    }
                }
            }
            fmt::print("OR \n");
            for (auto i : spelniajace_OR) {
                fmt::print("---{} \n", i);
            }

            fmt::print("\n\n ======= \n");

            //[!-- wykonanie zwiazanych z AND
            std::vector<int> spelniajace_AND;
            bool istnieje_AND=false;
            for (auto i=0; i < PARSED_logic_operators.size(); i++) {
                if (PARSED_logic_operators[i] == LOGIC_where::AND) {
                    istnieje_AND=true;
                    for (auto indexy : poprawne_indexy_wynikow[i]) {
                        for (auto indexy_prawa_strona : poprawne_indexy_wynikow[i+1]) {
                            if (indexy == indexy_prawa_strona) {
                                if (is_it_unique(spelniajace_AND, indexy)) {
                                    spelniajace_AND.push_back(indexy);
                                }
                            }
                        }
                    }
                }
            }
            fmt::print("\n\n AND \n");
            for (auto i : spelniajace_AND) {
                fmt::print("---{} \n", i);
            }

            if (istnieje_AND) {
                if (spelniajace_OR.size() == 0) {
                    spelniajaceANDiOR = spelniajace_AND;
                }
                else {
                    for (auto i : spelniajace_OR) {
                        for (auto y : spelniajace_AND) {
                            if (i == y) {
                                spelniajaceANDiOR.push_back(y);
                            }
                        }
                    }
                }
            }
            else {
                spelniajaceANDiOR = spelniajace_OR;
            }

            fmt::print("\n\n Wszystkie warunki \n");
            for (auto i : spelniajaceANDiOR) {
                fmt::print("---{} \n", i);
            }
        }
        else {
            spelniajaceANDiOR = poprawne_indexy_wynikow[0];
        }

        //[!-- Najprzymniejsza czesc, tworzenie nowej tablicy
        Table output_table_where;
        std::string das_table_name = FROM_args_FIELD[0] + " : ";
        for (auto i = 0; i < PARSED_logic_operators.size(); i++) {
            das_table_name += PARSED_where_statements[i].val1 + " " + Where_STATEMENT::operator_to_string(PARSED_where_statements[i].operatorWhere) + " " + PARSED_where_statements[i].val2;
            if (PARSED_logic_operators[i] == LOGIC_where::AND) {
                das_table_name += " AND ";
            }
            else {
                das_table_name += " OR ";
            }
        }
        das_table_name += PARSED_where_statements[PARSED_where_statements.size()-1].val1 + " " + Where_STATEMENT::operator_to_string(PARSED_where_statements[PARSED_where_statements.size()-1].operatorWhere) + " " + PARSED_where_statements[PARSED_where_statements.size()-1].val2;
        output_table_where.name = das_table_name;

        std::vector<Column> kolumny_do_table_where;
        for (auto i : this->db_user.get_table_by_name(FROM_args_FIELD[0]).columns) {
            Column serio_chwilowa_kolumna;
            std::vector<std::string> serio_chwilowy_wektor_wartosci;
            serio_chwilowa_kolumna.type = i.type;

            for (auto indexy : spelniajaceANDiOR) {
                serio_chwilowy_wektor_wartosci.push_back(i.values[indexy]);
            }
            serio_chwilowa_kolumna.values = serio_chwilowy_wektor_wartosci;
            kolumny_do_table_where.push_back(serio_chwilowa_kolumna);
        }
        output_table_where.columns = kolumny_do_table_where;
        database_methods::print_table(output_table_where);
    }

}
