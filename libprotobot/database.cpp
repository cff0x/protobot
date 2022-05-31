#include <protobot/database.h>

#include <utility>

database::database(std::string host, std::string username, std::string password, std::string dbName) {
    m_config = std::make_shared<postgresql::connection_config>();
    m_config->user = username;
    m_config->dbname = dbName;
    m_config->password = password;
    m_config->host = host;
    m_config->port = 5432;
    m_config->debug = true;
    m_connection.connectUsing(m_config);
}

std::string database::get_bot_option(const std::string &option) {
    protobot::BotOptions botOptions;
    return m_connection(select(botOptions.value).from(botOptions).where(botOptions.option == option)).front().value;
}

void database::set_bot_option(const std::string &option, const std::string &value) {
    protobot::BotOptions botOptions;
    m_connection(update(botOptions).set(botOptions.value = value).where(botOptions.option == option));
}

database::~database() = default;


