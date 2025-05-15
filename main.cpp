#include <iostream>
#include "Misc/app.h"
#include <fmt/base.h>
#include "Databases/database.h"
#include "query/query.h"

auto main()-> int {

    query_misc::tokenize("SELECT id.users, username.logs, guwno FROM users, logs");


    /*
    app user_session_app;
    user_session_app.setup();
    database user_database(user_session_app);

    //[!-- jesli user juz ma jakies bazy danych --!]
    if (!user_session_app.is_it_first_time) {
        fmt::print("\n[!] Your databases: \n");
        user_session_app.load_home_database();
    }

    //[!-- STAGE 1, ladowanie bazy --!]
    fmt::print("\n[?] How do u wanna load database? \n");
    do {
        if (!user_database.tables.empty()) {
            break;
        }
        int choice = 0;
        fmt::print(""
                   "    [1] Load database by pasting a path \n"
                   "    [2] Create database \n");
        if (!user_session_app.is_it_first_time) {
            fmt::print("    [3] Load database from home dir\n");
        }
        std::string path;

        std::cin >> choice;
        switch (choice) {
            case 1:
                fmt::print("[?] Give a path: ");
                std::cin >> path;
                user_database.load_database(path);

                break;
            case 2:
                fmt::print("[!] Creating new database\n");

                user_database.new_database_setup();
                break;
            case 3:
                if (!user_session_app.is_it_first_time) {
                    std::string choosen_database;
                    fmt::print("[?] Give a name: ");
                    std::cin >> choosen_database;

                    user_database.load_database(user_session_app.home_dir + "\\" + choosen_database);
                }
                else {
                    fmt::print("[!] Pick valid option\n");
                    continue;
                }
                break;
            default:
                fmt::print("[!] Pick valid option\n");
                continue;
        }

    }while (true);

    //[!-- STAGE 2, dzialania na bazie --!]
    fmt::print("\n[?] What u wanna do?? \n");
    fmt::print(""
           "    [1] Show everything from your table "
           "    [2] Query operations \n");

    */
}
