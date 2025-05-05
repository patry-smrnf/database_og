#include "database.h"

#include <filesystem>

#include "fmt/printf.h"
#include <fstream>
#include <iostream>
#include <string>

database::database(const app &session) {
    fmt::print("[+] Database init started \n");
    this->user_session = session;
}

void database::tables_setup(std::ofstream &database_file, std::vector<Table>& tables) {
    fmt::print("\n[*] Database Setup\n");

    std::vector<std::string> values;
    std::vector<Type> typy;
    Column temp_columna;
    Table temp_table;

    //[!-- Bedzie tworzyc nowe tablice w bazie az uzytkownik nie powie stop --!]
    for (auto i = 0; i > -1; i++) {
        //[!-- Czyszczenie zmiennych --!]
        typy.clear();

        std::string temp;
        fmt::print("[!] If u wanna stop, type 'exit'\n");
        fmt::print("    [?] Table number {} Name: ", i);
        std::cin >> temp;

        if (temp == "exit") {
            fmt::print("[x] Closing table setup\n");
            break;
        }

        //[!-- Stworzenie headeru z nazwa tablicy --!]
        database_file << database_creator::create_table_hedaer(temp) << "\n";
        temp_table.name = temp;

       //[!-- Stworzenie linijki z typami kolumn i ich nazwami --!]
        database_file << database_creator::create_types_header(typy) << "\n";

        //[!-- Zapelnianie tabelek w pliku --!]
        std::string choice = "";
        while (true) {
            fmt::print("[?] U wanna continue and fill the tables? y | n: ");
            std::cin >> choice;
            if (choice == "n") {
                break;
            }
            if (choice == "y") {
                database_file << database_creator::create_values_header(typy, values) << "\n";
            }
        }

        //[!-- Tworzenie kolumn --!]
        std::vector<std::vector<std::string>> helper_vector(typy.size());
        int counter_helper = 0;
        for (const auto & value : values) {
            helper_vector[counter_helper].push_back(value);

            counter_helper++;
            if (counter_helper == typy.size()) {
                counter_helper = 0;
            }
        }

        std::vector<Column> columns;
        for (auto y = 0; y < typy.size(); y++) {
            Column col;
            col.type = typy[y];               // assign corresponding Type
            col.values = helper_vector[y];    // assign column values
            columns.push_back(col);
        }

        temp_table.columns = columns; // Assign to the table
        tables.push_back(temp_table);
    }

    database_file.close();

}


void database::new_database_setup() {
    namespace fs = std::filesystem;

    std::vector<Table> Meine_tables;

    fmt::print("[*] Database Setup\n[?] Enter database name: ");
    std::string database_name;
    std::cin >> database_name;

    std::ofstream database_file(this->user_session.home_dir + "\\" + database_name + ".ogbase", std::ios::app);
    this->tables_setup(database_file, Meine_tables);

    std::string choice;
    fmt::print("[+] Database setup done\n[?] You want to work on current database?");
    std::cin >> choice;

    if (choice == "y") {
        this->tables = Meine_tables;
    }
}

void database::load_database(const std::string &path) {
    fmt::print("\n[*] Database loader\n");

    //[!-- Zmienne dotyczace wartosci --!]
    std::string TEMP_name_table = "";
    std::vector<Type> types;
    std::vector<std::vector<std::string>> values_NOT_SORTED;

    //[!-- Zmienne dotyczace czytania pliku --!]
    auto file = std::ifstream(path);
    std::string line = "";

    //[!-- Tu sie bedzie dziac --!]
    if (!std::filesystem::exists(path)) {
        fmt::print("[-] Database does not exist\n");
        exit(0);
    }

    while (std::getline(file, line)) {

        // Header zawsze jest na poczatku kazdego table, sprawdzam czy zaczyna sie
        if (database_reader::is_it_header(line)) {

            //zapisywanie starej tablicy do obiektu gdy nadchodzi druga tablica
            if ((line.substr(1, line.length() -2) != TEMP_name_table) && (TEMP_name_table != "")) {
                this->tables.push_back(database_reader::make_it_table(TEMP_name_table, types, values_NOT_SORTED));
                types.clear();
                values_NOT_SORTED.clear();
            }
            //fmt::print("[!] Table Found!\n");
            TEMP_name_table = line.substr(1, line.length() - 2);
            //fmt::print("[+] Table name: {}\n", TEMP_name_table);
        }

        // Row z typami otoczony {  },
        if (database_reader::is_it_types_row(line)) {
            std::string row_types = line.substr(1, line.length() - 2); // Usuniecie nawiasow
            types = database_reader::parse_fields(row_types); //wpisanie wszystkich typow do wektora
        }

        if (!database_reader::is_it_header(line) && !database_reader::is_it_types_row(line)) {
            values_NOT_SORTED.push_back(database_reader::parse_values_row(line));
        }
    }
    this->tables.push_back(database_reader::make_it_table(TEMP_name_table, types, values_NOT_SORTED));

    fmt::print("\n[+] {} Loaded succefully\n", path);

}

