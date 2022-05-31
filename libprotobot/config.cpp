#include <protobot/config.h>
#include <fstream>

config::config(std::filesystem::path file_path) :
        m_file_path(std::move(file_path)) {
    std::ifstream configFile(m_file_path);
    configFile >> m_json_data;
    configFile.close();
}

config::~config() = default;

void config::reload() {
    std::ifstream configFile(m_file_path);
    configFile >> m_json_data;
    configFile.close();
}

void config::set_modules_path(std::string modules_path) {
    m_modules_path = modules_path;
}

std::string config::get_modules_path() {
    return m_modules_path;
}

json config::data() {
    return m_json_data;
}

const std::string config::token() {
    return m_json_data["discord"]["token"].get<std::string>();
}

const dpp::snowflake config::application_id() {
    return m_json_data["discord"]["appId"].get<dpp::snowflake>();
}

const dpp::snowflake config::guild_id() {
    return m_json_data["discord"]["guildId"].get<dpp::snowflake>();
}

const std::string config::database_host() {
    return m_json_data["database"]["host"].get<std::string>();
}

const std::string config::database_username() {
    return m_json_data["database"]["username"].get<std::string>();
}

const std::string config::database_password() {
    return m_json_data["database"]["password"].get<std::string>();
}

const std::string config::database_db_name() {
    return m_json_data["database"]["dbname"].get<std::string>();
}