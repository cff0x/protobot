#ifndef PROTOBOT_TEST_MODULE2_H
#define PROTOBOT_TEST_MODULE2_H

#include <protobot/client.h>
#include <protobot/module.h>

class test_module_2 : public bot_module
{
public:
    test_module_2(client* bot, bot_module_manager* moduleManager);
    virtual ~test_module_2();

    virtual std::string get_version();
    virtual std::string get_description();

    virtual std::vector<module_command> get_commands();

    virtual bool on_slashcommand(const dpp::slashcommand_t &cmd);
    virtual bool on_ready(const dpp::ready_t &ready);
    virtual bool on_button_click(const dpp::button_click_t &btn);

    void on_command_test(const dpp::slashcommand_t &cmd);
};

MODULE_ENTRY(test_module_2);

#endif //PROTOBOT_TEST_MODULE2_H
