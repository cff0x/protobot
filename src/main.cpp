#include <protobot/protobot.h>
#include <protobot/config.h>
#include <protobot/client.h>
#include <sstream>
#include <algorithm>
#include <exception>
#include <signal.h>

#include <CLI/CLI.hpp>
//#include <getopt.h>

int main(int argc, char const *argv[])
{
    CLI::App app("ProtoBot Discord Bot");
    app.set_version_flag("--version", "0.420.69");

    // application parameters
    std::string configFile{"config/config.json"};
    std::string logFile{"logs/protobot.log"};
    std::string modulesPath{"modules"};
    bool devMode{false};

    app.add_option("-m,--modules-path,modules-path", modulesPath, "Folder containing the bot modules");
    app.add_option("-c,--config,config", configFile, "JSON config file");
    app.add_option("-l,--logfile,logfile", logFile, "Log output file");
    app.add_option("-d,--dev,dev", devMode, "Developer mode");

    CLI11_PARSE(app, argc, argv);

    // read config file
    config config{configFile};
    config.set_modules_path(modulesPath);

    // setup bot client
    uint32_t intents = dpp::i_default_intents;
    intents |= dpp::i_message_content;
    dpp::cluster bot(config.token(), intents);

    // create database connection
    database database{config.database_host(),
                      config.database_username(),
                      config.database_password(),
                      config.database_db_name()};

    client client(&bot, &config, &database, devMode);

    // Set up spdlog logger
    std::shared_ptr<spdlog::logger> log;
    spdlog::init_thread_pool(8192, 2);
    std::vector<spdlog::sink_ptr> sinks;
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
    auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile, 1024 * 1024 * 5, 10);
    sinks.push_back(stdout_sink);
    sinks.push_back(rotating);
    log = std::make_shared<spdlog::async_logger>("test", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    spdlog::register_logger(log);
    log->set_pattern("%^%Y-%m-%d %H:%M:%S.%e [%L] [th#%t]%$ : %v");
    log->set_level(spdlog::level::level_enum::debug);

    // Integrate spdlog logger to D++ log events
    bot.on_log([&bot, &log](const dpp::log_t & event) {
        switch (event.severity) {
            case dpp::ll_trace:
                log->trace("{}", event.message);
                break;
            case dpp::ll_debug:
                log->debug("{}", event.message);
                break;
            case dpp::ll_info:
                log->info("{}", event.message);
                break;
            case dpp::ll_warning:
                log->warn("{}", event.message);
                break;
            case dpp::ll_error:
                log->error("{}", event.message);
                break;
            case dpp::ll_critical:
            default:
                log->critical("{}", event.message);
                break;
        }
    });

    if(devMode) {
        client.log(dpp::ll_info, "=== RUNNING IN DEVELOPER MODE ===");
    }

    // load modules
    client.load_modules();

    // bind events
    client.bind_events();

    try {
        // connect and start the event loop
        bot.start(false);
    }
    catch (std::exception e) {
        bot.log(dpp::ll_error, fmt::format("Oof! {}", e.what()));
    }

    std::cout << "LOL" << std::endl;

    return 0;
}
