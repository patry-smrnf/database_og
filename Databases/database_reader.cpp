#include <regex>
#include <sstream>

#include "database.h"
#include <fmt/base.h>

bool database_reader::is_it_header(const std::string& line) {
    if (line[0] == '[' && line[line.length() - 1] == ']') {
        return true;
    }
    return false;
}

bool database_reader::is_it_types_row(std::string const line) {
    if (line[0] == '{' && line[line.length() - 1] == '}') {
        return true;
    }
    return false;
}


std::vector<Type> database_reader::parse_fields(std::string const line) {
    std::vector<Type> fields;
    std::regex field_pattern(R"(\((\w+)\)(\w+))");
    auto begin = std::sregex_iterator(line.begin(), line.end(), field_pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        fields.push_back({(*it)[1].str(), (*it)[2].str()});
    }

    return fields;
}

std::vector<std::string> database_reader::parse_values_row(std::string line) {
    std::vector<std::string> values;

    std::stringstream ss(line);
    std::string value;
    while (std::getline(ss, value, ',')) {
        values.push_back(value);
    }

    return values;
}

Table database_reader::make_it_table(std::string name, std::vector<Type> types, std::vector<std::vector<std::string>> values) {
    Table table_output;
    table_output.name = name;

    std::vector<Column> kolumny;

    const int ilosc_typow = types.size();

    //sprawdzenie czy jest tyle wartosci co typow, wszedzie powinno byc po rowno
    for (auto i : values) {
        if (i.size() != ilosc_typow) {
            fmt::print("[-] Nie zgadza sie ilosc wartosci, sprawdz czy nigdzie nie ma ',' na koncu");
            exit(0);
        }
    }


    for (auto i = 0; i < ilosc_typow; i++) {
        //[!-- Tymczasowe Zmienne ktore uzupelniaja --!]
        Column column_for_table;
        std::vector<std::string> wartosci;

        // Zaladowanie typu do kolumny
        column_for_table.type = types[i];

        // Wczytywanie danych
        for (auto y = 0; y < values.size(); y++) {
            wartosci.push_back(values[y][i]);
        }
        // Ladowanie danych do kolumny
        column_for_table.values = wartosci;

        // Dodanie kolumny do tablicy kolumn dla tablicy
        kolumny.push_back(column_for_table);
    }

    // Zapisanie kolumn do otatecznej tablicy
    table_output.columns = kolumny;

    return table_output;
}

