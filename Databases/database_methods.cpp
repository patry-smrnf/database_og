#include "database.h"
#include "fmt/printf.h"

void database_methods::print_table_details(Table& table) {
    std::vector<std::string> column_names;

    for (auto i : table.columns) {
        column_names.push_back(i.type.name);
    }

    auto max_size = column_names.size() * 7;
    for (auto i : column_names) {
        max_size += i.size();
    }

    for (auto i = 0; i < max_size; i++) {
        fmt::print("=");
    }
    fmt::print("\nTable Name: {}\n", table.name);
    for (auto i = 0; i < max_size; i++) {
        fmt::print("-");
    }
    fmt::print("\n");
    for (auto i : column_names) {
        fmt::print("   {}   |", i);
    }
    fmt::print("\n");
    for (auto i = 0; i < max_size; i++) {
        fmt::print("=");
    }
    fmt::print("\n");

    for (auto j = 0; j < table.columns[0].values.size(); j++) {
        for (auto i = 0; i < table.columns.size(); i++) {
            fmt::print("   {}    ", table.columns[i].values[j]);
        }
        fmt::print("\n");

    }

}
