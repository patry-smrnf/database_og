#include "app.h"
#include <filesystem>
#include <iostream>

#include "fmt/base.h"

void app::load_home_database() {
    fmt::print("[+] Loading home database...\n");
    namespace fs = std::filesystem;

    for (const auto & entry : fs::directory_iterator(app::home_dir)) {
        fmt::print("    [*] {}\n", entry.path().filename().string());
        app::home_databases.push_back(entry.path().string());
    }
}

void app::setup() {
    //[!-- filesystem przyda sie w czytaniu folderow/plikow --!]
    namespace fs = std::filesystem;

    fmt::print("[*] OG DATABASE setup\n\n[*] Checking if home dir exists\n");

    if (!fs::exists(app::home_dir)) {
        app::is_it_first_time = true;
        fmt::print("[-] Home directory does not exist - Creating it\n");
        fs::create_directory(app::home_dir);
    }
}
app::app() {

}
