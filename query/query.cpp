#include "query.h"

#include <algorithm>
#include <map>
#include <sstream>

#include "fmt/printf.h"

bool token_detector(std::string word) {
    std::ranges::transform(word, word.begin(), ::toupper);

    if (auto it = token_map.find(word); it != token_map.end()) {
        return true;
    }
    return false;

}

void query_misc::tokenize(const std::string &RAWquery) {

    std::istringstream stream(RAWquery);
    std::string token;

    std::vector<command> commands_OUT;
    command temp_command;
    std::vector<std::string> argumenty_TEMP;

    while (stream >> token) {
        std::erase(token, ',');

        if (token_detector(token)) {
            if (!temp_command.name.empty()) {
                temp_command.args = argumenty_TEMP;
                argumenty_TEMP.clear();
                commands_OUT.push_back(temp_command);
            }
            temp_command.name = token;
        }
        else {
            argumenty_TEMP.push_back(token);
        }
    }
    temp_command.args = argumenty_TEMP;
    commands_OUT.push_back(temp_command);


    for (auto i : commands_OUT) {
        fmt::print(" - {}\n", i.name);
        for (auto y : i.args) {
            fmt::print("     - {}\n", y);
        }
    }
}
