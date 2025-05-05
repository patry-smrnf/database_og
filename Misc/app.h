#ifndef APP_H
#define APP_H
#include <string>
#include <vector>

class app {
public:
    app();

    //[!-- Wewnetrzne funkcje --!]
    void load_home_database();
    void setup();
    //[!-- Zmienne co sa ogolne --!]
    std::string home_dir = "C:\\og_base";
    bool is_it_first_time = false;
    std::vector<std::string> home_databases;


};

#endif //APP_H
