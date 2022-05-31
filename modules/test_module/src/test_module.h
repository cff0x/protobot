#ifndef PROTOBOT_TEST_MODULE_H
#define PROTOBOT_TEST_MODULE_H

#include <protobot/client.h>
#include <protobot/module.h>

using namespace protobot;

class test_module : public bot_module
{
public:
    test_module(client* bot, bot_module_manager* moduleManager);
    virtual ~test_module();

    virtual std::string get_version();
    virtual std::string get_description();

    virtual std::vector<module_command> get_commands();

    virtual bool on_slashcommand(const dpp::slashcommand_t &cmd);
    virtual bool on_ready(const dpp::ready_t &ready);
    virtual bool on_button_click(const dpp::button_click_t &btn);

    void on_command_test(const dpp::slashcommand_t &cmd);
};

MODULE_ENTRY(test_module);

#endif //PROTOBOT_TEST_MODULE_H
