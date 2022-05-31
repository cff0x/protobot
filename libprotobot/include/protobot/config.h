#ifndef __PROTOBOT__CONFIG_H__
#define __PROTOBOT__CONFIG_H__

#include <dpp/nlohmann/json.hpp>
#include <filesystem>
#include <dpp/snowflake.h>

using json = nlohmann::json;

class config {
public:
    config(std::filesystem::path file_path);
    ~config();

    void reload();
    json data();

    void set_modules_path(std::string modules_path);
    std::string get_modules_path();

    const std::string token();
    const dpp::snowflake application_id();
    const dpp::snowflake guild_id();

    const std::string database_host();
    const std::string database_username();
    const std::string database_password();
    const std::string database_db_name();

private:
    std::string m_modules_path;
    std::filesystem::path m_file_path;
    json m_json_data;
};

#endif //__PROTOBOT__CONFIG_H__
