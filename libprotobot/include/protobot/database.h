#ifndef PROTOBOT_DATABASE_H
#define PROTOBOT_DATABASE_H
#include <protobot/protobot.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/postgresql.h>
#include <protobot/ddl/bot_options.h>

namespace protobot {
using namespace protobot;
namespace postgresql = sqlpp::postgresql;

class database {
public:
    database(std::string host, std::string username, std::string password, std::string dbName);

    ~database();

    std::string get_bot_option(const std::string &option);

    void set_bot_option(const std::string &option, const std::string &value);

private:
    std::shared_ptr<postgresql::connection_config> m_config;
    postgresql::connection m_connection;
};
}

#endif //PROTOBOT_DATABASE_H
