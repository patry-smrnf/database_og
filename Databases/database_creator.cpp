#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include "database.h"
#include "database_structures.h"
#include "fmt/base.h"

data_types get_data_type_from_string(const std::string& input) {
    std::string s = input;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);

    if (s == "INTEGER") return data_types::INTEGER;
    if (s == "VARCHAR") return data_types::VARCHAR;
    if (s == "TIMESTAMP") return data_types::TIMESTAMP;
    return data_types::INVALID;
}


std::string database_creator::create_table_hedaer(const std::string& name) {
    return "[" + name + "]";
}

std::string database_creator::create_types_header(std::vector<Type>& typy) {
    std::string output = "{";
    fmt::print("[!] If u wanna stop, type 'exit'\n");

    std::string key_name;
    std::string key_type;

    while (true) {
        fmt::print("    [?] Key name: ");
        std::cin >> key_name;
        if (key_name == "exit") {
            break;
        }
        fmt::print("    [?] Key type: ");
        std::cin >> key_type;

        data_types dtype = get_data_type_from_string(key_type);

        if (dtype == data_types::INVALID) {
            std::cout << "[-] Invalid type Try again.\n";
        }
        else {
            typy.push_back({key_type, key_name});
            output += "(" + key_type + ")" + key_name + ",";
        }
    }
    output.pop_back();
    output += "}";

    return output;
}

std::string database_creator::create_values_header(std::vector<Type> typy, std::vector<std::string>& values) {
    std::string output;
    std::string temp;

    for (auto i : typy) {
        fmt::print("[*] Type name: {}\n"
                   "    [?] Value: ", i.name);
        std::cin >> temp;
        output += temp + ",";
    }

    output.pop_back();

    return output;
}
