#include <fstream>
#include <iostream>
#include "Misc/app.h"
#include <fmt/base.h>
#include "Databases/database.h"
#include "DDL/DDL.h"
#include "DML/DML.h"
#include "DQL/DQL.h"


void create(std::string path) {

    std::ofstream file(path);  // Create or overwrite file
    fmt::print("{}", path);
    file.close();
}

auto main()-> int {

    /*
    app user_session_app;
    user_session_app.setup();
    database user_database(user_session_app);
    user_database.load_database("C:\\og_base\\testowa2.ogbase");

    std::string komenda;
    std::getline(std::cin, komenda);  // reads entire line including spaces
    DQL_obj user_query(user_database);
    user_query.exec_dql(komenda);


    // DML_obj user_query(user_database);
    // user_query.exec_dml("INSERT niggers VALUES ( sraka niewie )");


    */


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
                user_database.load_database(path);

                break;
            case 2:
                fmt::print("[?] Do u want create a database by\n1. later SQLlike QUERY\n2. Setup: ");
                std::cin >> choice;
                if (choice == 1) {
                    fmt::print("[?] Give a name: ");
                    std::cin >> path;
                    create(user_session_app.home_dir + "\\" + path + ".ogbase");
                    user_database.load_database(user_session_app.home_dir + "\\" + path + ".ogbase");
                }
                else {
                    fmt::print("[!] Creating new database\n");
                    user_database.new_database_setup();
                }

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

    fmt::print("\n[+] Your tables? \n");
    for (auto i : user_database.tables) {
        fmt::print("- {} \n", i.name);
    }
    do {
        DML_obj dml_obj(user_database);
        DQL_obj dql_obj(user_database);
        DDL_obj ddl_obj(user_database);

        int choice = 0;
        //[!-- STAGE 2, dzialania na bazie --!]
        fmt::print("\n[?] Which QUERY u do?? \n");
        fmt::print(""
               "    [1] DDL \n"
               "    [2] DQL \n"
               "    [3] DML \n"
               "    [4] Exit \n");

        int choice_2 = 0;
        std::cin >> choice_2;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string old_path = user_database.path;

        if (choice_2 == 1) {
            fmt::print("[ data definition language ] \n Usage: CREATE table1 ( username VARCHAR id INTEGER ) \n DROP table2 \n\n");
            std::string command;

            std::getline(std::cin, command);
            ddl_obj.exec_ddl(command);
            user_database.load_database(old_path);
        }
        if (choice_2 == 2) {
            fmt::print("[ data query language ] \n Usage: SELECT * FROM table1 \n SELECT user FROM table2 WHERE user = 'admin' OR \n\n");
            std::string command;

            std::getline(std::cin, command);
            dql_obj.exec_dql(command);
        }
        if (choice_2 == 3) {
            fmt::print("[ data modification language ] \n Usage: INSERT table1 VALUES ( 0 tijara password ) ( 1 sentino mojehaslo )\n\n");
            std::string command;

            std::getline(std::cin, command);
            dml_obj.exec_dml(command);
            user_database.load_database(old_path);
        }
        if (choice_2 == 4) {
            fmt::print("bye bye user\n\n");
            exit(0);
        }
    }while (true);



}
