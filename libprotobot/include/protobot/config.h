#ifndef __PROTOBOT__CONFIG_H__
#define __PROTOBOT__CONFIG_H__

#include <dpp/nlohmann/json.hpp>
#include <filesystem>
#include <dpp/snowflake.h>

namespace protobot {
using namespace protobot;
using json = nlohmann::json;

class config {
public:
    config(std::filesystem::path file_path);

    ~config();

    void reload();

    json data();

    void set_modules_path(std::string modules_path);

    std::string get_modules_path();

    std::string token();

    dpp::snowflake dev_guild_id();

    std::string database_host();

    std::string database_username();

    std::string database_password();

    std::string database_db_name();

private:
    std::string m_modules_path;
    std::filesystem::path m_file_path;
    json m_json_data;
};

}

#endif //__PROTOBOT__CONFIG_H__
